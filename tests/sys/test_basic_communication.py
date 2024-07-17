import logging
import pytest
from tests.sys.fixtures import rpc_client


LOGGER = logging.getLogger(__name__)


@pytest.mark.usefixtures("rpc_client")
class TestBasicRPCCommunication:
    """ Tests basic communication with the RPC server. """

    def test_ping(self, rpc_client):
        """ Tests the ping command. """
        assert rpc_client.ping()
