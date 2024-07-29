import logging
import pytest
from tests.sys.fixtures import client

LOGGER = logging.getLogger(__name__)


# @pytest.mark.parametrize("client", ["INFO"], indirect=True)
class TestBasicRPCCommunication:
    """ Tests basic communication with the RPC server. """

    def test_ping(self, client):
        """ Tests the ping command across all nodes in the system. """
        for node in client.available_nodes:
            assert client.ping_node(node), f"Node {node} did not respond to ping"

    def test_query_sensor_data(self, client):
        """ Tests querying sensor data from all nodes in the system. """
        for node in client.available_nodes:
            assert client.get_input_voltage(node) is not None, f"Node {node} did not return valid input voltage"
            assert client.get_output_voltage(node) is not None, f"Node {node} did not return valid output voltage"
