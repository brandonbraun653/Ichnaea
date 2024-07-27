from time import sleep

from ichnaea.ichnaea_client import IchnaeaClient

if __name__ == "__main__":
    client = IchnaeaClient(port="/dev/ttyACM1", baud=115200)
    client.connect()

    print("Identifying nodes")
    nodes = client.identify_nodes()
    for node in nodes:
        print(f"Node {node[0]} has firmware version {node[1]}")

    client.disconnect()
