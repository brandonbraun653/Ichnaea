import sys
from time import sleep
from loguru import logger
from ichnaea.ichnaea_client import IchnaeaClient

if __name__ == "__main__":
    logger.remove()
    logger.add(sys.stderr, level="INFO")

    client = IchnaeaClient(port="/dev/ttyACM1", baud=115200)
    client.open()

    ic = client.available_nodes[0]
    if not client.ping_node(ic):
        logger.error(f"Node {ic} is not responding")

    client.close()
