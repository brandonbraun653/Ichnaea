import logging

import pytest
from tests.sys.fixtures import *
from ichnaea.proto.ichnaea_pdi_pb2 import *

LOGGER = logging.getLogger(__name__)


class TestNodeStateInspection:
    """Test ability to inspect various state descriptors of an Ichnaea node."""

    @pytest.fixture(autouse=True)
    def setup_method(self, node_link: NodeClient, product_config):
        """Common setup routines before each test case"""
        self.node_link: NodeClient = node_link
        self.platform: Platform = product_config["type"]

    def test_liveness(self):
        """Test the liveness of the node."""
        assert self.node_link.is_alive()


class TestNodeCommands:
    """Test the basic commands an Ichnaea node should respond to."""

    @pytest.fixture(autouse=True)
    def setup_method(self, node_link: NodeClient, product_config):
        """Common setup routines before each test case"""
        self.node_link: NodeClient = node_link
        self.platform: Platform = product_config["type"]

    def test_reboot(self):
        """Test rebooting the node."""
        prev_boot_count = self.node_link.pdi_read(PDI_ID.BOOT_COUNT).boot_count
        self.node_link.reboot()
        new_boot_count = self.node_link.pdi_read(PDI_ID.BOOT_COUNT).boot_count
        assert new_boot_count > prev_boot_count

    def test_sleep_on_node_time(self):
        """Test we can sleep the test based on the system node time"""
        try:
            self.node_link.sleep_on_node_time(5)
        except Exception as e:
            LOGGER.error(f"Exception occurred: {e}")
            assert False
