from time import sleep

from mbedutils.rpc.client import RPCClient

if __name__ == "__main__":
    client = RPCClient()
    client.open(port="/dev/ttyACM1", baud=115200)
    sleep(1)
    if not client.ping():
        print("Failed to ping the server")

    sleep(5)
    client.close()
