import sys
from time import sleep
from loguru import logger
from ichnaea.ichnaea_client import IchnaeaClient

if __name__ == "__main__":
    logger.remove()
    logger.add(sys.stderr, level="INFO")

    client = IchnaeaClient(port="/dev/ttyACM1", baud=115200)
    client.open()

    if not client.available_nodes:
        logger.error("No nodes found in the system")
        client.close()
        sys.exit(1)

    logger.info(f"Available nodes: {client.available_nodes}")

    node = client.available_nodes[0]
    if not client.ping_node(node):
        logger.error(f"Node {node} is not responding")

    client.engage_output(node)
    client.set_output_voltage(node, 12.0)
    sleep(0.5)
    voltage = client.get_output_voltage(node)
    logger.info(f"Output voltage: {voltage:.2f}V")

    sleep(10000)
    client.disengage_output(node)
    client.close()
