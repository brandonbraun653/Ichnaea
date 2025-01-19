import os
import signal
import subprocess
import sys
import time
from enum import IntEnum
from pathlib import Path
from typing import Generator

import pytest
from loguru import logger

from ichnaea.network_client import NetworkClient
from ichnaea.node_client import NodeClient
from mbedutils.rpc.pipe import PipeType


class Platform(IntEnum):
    Hardware = 0
    Simulator = 1


HARDWARE_CONFIG = {"type": Platform.Hardware, "port": "/dev/ttyACM1", "baudrate": 115200}
SIMULATOR_CONFIG = {
    "type": Platform.Simulator,
    "host": "localhost",
    "port": "ipc:///tmp/ichnaea_sim/5556",
    "sim_path": (Path(__file__).parent.parent.parent / "artifacts" / "host" / "Debug" / "Ichnaea").as_posix(),
    "sim_debug": False,
}

# Used to mark tests that should be run serially. Typically these are integration Sim/HW tests
# which depend on a file or resource that can't be shared across multiple tests.
#
# Mark the test using:
# @pytest.mark.xdist_group(name=pytest_serial_executor)
pytest_serial_executor = "serial_executor"


@pytest.fixture(scope="session", params=["hardware", "simulator"])
def product_config(request):
    if request.param == "hardware":
        return HARDWARE_CONFIG
    elif request.param == "simulator":
        return SIMULATOR_CONFIG


@pytest.fixture(scope="session")
def simulator(product_config):
    """
    Pytest fixture to start the simulator, if configured.
    """
    if product_config["type"] != Platform.Simulator:
        yield 0
        return

    if product_config.get("sim_debug", False):
        logger.info("Configured to debug the simulator. Ensure it's open elsewhere.")
        yield 0
        return

    if "sim_path" not in product_config or not Path(product_config["sim_path"]).exists():
        raise RuntimeError("Simulator binary not found. Skipping test.")

    # First, kill all running instances of the simulator
    try:
        subprocess.run(["pkill", "-x", "Ichnaea"], check=True)
    except subprocess.CalledProcessError:
        pass  # No issues. The process wasn't running.

    # Start the simulator
    try:
        sim_process = subprocess.Popen(
            [product_config["sim_path"]], stderr=subprocess.PIPE, cwd=Path(product_config["sim_path"]).parent
        )
        time.sleep(0.25)
        if sim_process.poll() is not None:
            stderr = sim_process.communicate()[1].decode("utf-8").strip()
            raise RuntimeError(f"Failed to start the simulator: {stderr}")
    except Exception as e:
        raise RuntimeError(f"Failed to start the simulator: {e}")

    yield sim_process

    # Gracefully exit the simulator
    sim_process.terminate()
    try:
        sim_process.wait(timeout=5)
    except subprocess.TimeoutExpired:
        os.kill(sim_process.pid, signal.SIGKILL)


@pytest.fixture(scope="session")
def persistent_connection(product_config, simulator) -> Generator[NetworkClient, None, None]:
    """
    Pytest fixture to create a persistent serial connection to the product.

    This fixture will use the configuration provided by the `product_config`
    fixture to connect to either the hardware or the simulator, and the
    connection will be shared across all tests in the session.
    """
    log_level = "TRACE"
    logger.remove()
    logger.add(sys.stderr, level=log_level)

    if product_config["type"] == Platform.Hardware:
        pipe_type = PipeType.DEVICE
    else:
        pipe_type = PipeType.ZMQ

    connection = NetworkClient(
        pipe_type=pipe_type, port=product_config["port"], baud=product_config.get("baudrate", 115200)
    )
    connection.open()
    connection.discover_nodes(exp_count=1, timeout=5.0)
    if len(connection.available_nodes) == 0:
        connection.close()
        pytest.fail("No nodes found in the system", pytrace=False)
    yield connection
    connection.close()


@pytest.fixture
def node_link(persistent_connection) -> Generator[NodeClient, None, None]:
    """
    Pytest fixture to provide a test-scoped connection.

    This fixture wraps the session-scoped `persistent_connection` fixture
    and can be used to perform additional setup or configuration on a
    per-test basis.
    """
    node_id = persistent_connection.available_nodes[0]
    node = NodeClient(node_id, persistent_connection)
    yield node
