import sys
import pytest
from loguru import logger
from mbedutils.rpc.client import RPCClient


@pytest.fixture
def rpc_client() -> RPCClient:
    logger.remove()
    logger.add(sys.stderr, level="ERROR")
    client = RPCClient()
    # client = OrbitClient(port=37218)

    client.open(port="/dev/ttyACM1", baud=115200)
    yield client
    client.close()