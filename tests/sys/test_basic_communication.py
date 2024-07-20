import logging
import pytest
from tests.sys.fixtures import rpc_client

LOGGER = logging.getLogger(__name__)


# @pytest.mark.parametrize("rpc_client", ["TRACE"], indirect=True)
class TestBasicRPCCommunication:
    """ Tests basic communication with the RPC server. """

    def test_ping(self, rpc_client):
        """ Tests the ping command. """
        assert rpc_client.ping()
