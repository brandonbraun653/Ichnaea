import logging
import pytest

from tests.sys.fixtures import *
from ichnaea.proto.ichnaea_pdi_pb2 import *
from mbedutils.rpc.proto.mbed_rpc_pb2 import LoggerWriteRequest

LOGGER = logging.getLogger(__name__)


class TestLogInspection:
    """Tests basic communication with the RPC server."""

    @pytest.fixture(autouse=True)
    def setup_method(self, node_link: NodeClient, product_config):
        """Common setup routines before each test case"""
        self.node_link: NodeClient = node_link
        self.platform: Platform = product_config["type"]

    def test_erase_log(self):
        """Test erasing the NVM log on-device."""
        assert self.node_link.log_erase(), "Failed to erase the log"

    def test_write_log(self):
        """Test writing a log entry to the device."""
        if self.platform == "simulator":
            assert self.node_link.log_erase()
        assert self.node_link.log_write(
            "Test log entry", level=LoggerWriteRequest.Level.LEVEL_FATAL
        ), "Failed to write log entry"

    def test_read_log(self):
        """Test reading a log entry from the device."""
        if self.platform == "simulator":
            assert self.node_link.log_erase()
        assert self.node_link.log_write(message="Hello, world!", level=LoggerWriteRequest.Level.LEVEL_FATAL)
        logs = self.node_link.log_read()
        assert len(logs) == 1
        assert logs[0] == "Hello, world!"

    def test_read_log_multiple(self):
        """Test reading multiple log entries from the device."""
        if self.platform == "simulator":
            assert self.node_link.log_erase()
        assert self.node_link.log_write(message="Hello, world!", level=LoggerWriteRequest.Level.LEVEL_FATAL)
        assert self.node_link.log_write(message="Goodbye, world!", level=LoggerWriteRequest.Level.LEVEL_FATAL)
        logs = self.node_link.log_read(count=2)
        assert len(logs) == 2
        assert logs[0] == "Hello, world!"
        assert logs[1] == "Goodbye, world!"

    def test_read_log_multiple_reverse(self):
        """Test reading multiple log entries from the device in reverse order."""
        if self.platform == "simulator":
            assert self.node_link.log_erase()
        assert self.node_link.log_write(message="Hello, world!", level=LoggerWriteRequest.Level.LEVEL_FATAL)
        assert self.node_link.log_write(message="Goodbye, world!", level=LoggerWriteRequest.Level.LEVEL_FATAL)
        logs = self.node_link.log_read(count=2, direction=False)
        assert len(logs) == 2
        assert logs[0] == "Goodbye, world!"
        assert logs[1] == "Hello, world!"
