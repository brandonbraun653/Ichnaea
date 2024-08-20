import sys
import pytest
from loguru import logger

from ichnaea.ichnaea_client import IchnaeaClient


@pytest.fixture
def client(request) -> IchnaeaClient:
    log_level = request.param if hasattr(request, "param") else "ERROR"
    logger.remove()
    logger.add(sys.stderr, level=log_level)
    client = IchnaeaClient(port="/dev/ttyACM1", baud=115200)

    client.open()
    assert len(client.available_nodes) > 0, "No nodes found in the system"
    yield client
    client.close()
