import logging
import pytest
from tests.sys.fixtures import client

LOGGER = logging.getLogger(__name__)


@pytest.mark.parametrize("client", ["TRACE"], indirect=True)
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

    def test_rx_overwhelm_with_requests(self, client):
        """ Tests overwhelming the RX buffer with requests. """
        assert len(client.available_nodes) > 0, "No nodes found in the system"

        for node in client.available_nodes:
            for x in range(100):
                assert client.ping_node(node), f"Node {node} did not respond to ping {x}"
            for x in range(100):
                assert client.get_input_voltage(node) is not None, f"Node {node} did not return valid input voltage {x}"