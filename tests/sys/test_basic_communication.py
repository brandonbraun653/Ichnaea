import logging
import pytest
from tests.sys.fixtures import rpc_client

LOGGER = logging.getLogger(__name__)


@pytest.mark.parametrize("rpc_client", ["TRACE"], indirect=True)
class TestBasicRPCCommunication:
    """ Tests basic communication with the RPC server. """

    def test_ping(self, rpc_client):
        """ Tests the ping command. """
        assert rpc_client.ping()

    def test_logging(self, rpc_client):
        from time import sleep, time
        LOGGER.debug("This is a debug message")
        # TODO BMB: Get the console logging to work, then start adding messages to pull data from the system
        #   and to inject commands for control.
