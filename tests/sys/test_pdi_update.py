import logging
import random

import pytest

from ichnaea.node_client import pdi_id_type_map
from tests.sys.fixtures import *
from ichnaea.proto.ichnaea_pdi_pb2 import *

LOGGER = logging.getLogger(__name__)


class TestPDIUpdate:
    """Test the ability to update PDI values on the Ichnaea node."""

    @pytest.fixture(autouse=True)
    def setup_method(self, node_link: NodeClient, product_config):
        """Common setup routines before each test case"""
        self.node_link: NodeClient = node_link
        self.platform: Platform = product_config["type"]

    def test_read_all_pdi(self):
        """Test reading all PDI values."""
        try:
            for pdi_id in pdi_id_type_map.keys():
                pdi = self.node_link.pdi_read(pdi_id)
        except Exception as e:
            pytest.fail(f"Error reading PDI: {e}")

    def test_boot_count_update(self):
        """Test updating the PID boot count."""
        # Read the current boot count
        old_pdi = self.node_link.pdi_read(PDI_ID.BOOT_COUNT)
        assert isinstance(old_pdi, PDI_BootCount)
        LOGGER.info(f"Read boot count: {old_pdi.boot_count}")

        # Program a new wild boot count
        pgm_pdi = PDI_BootCount()
        pgm_pdi.boot_count = random.Random().randint(old_pdi.boot_count, 65000)
        LOGGER.info(f"Writing boot count: {pgm_pdi.boot_count}")
        self.node_link.pdi_write(PDI_ID.BOOT_COUNT, pgm_pdi)

        # Read the new boot count
        new_pdi = self.node_link.pdi_read(PDI_ID.BOOT_COUNT)
        assert isinstance(new_pdi, PDI_BootCount)
        assert new_pdi.boot_count == pgm_pdi.boot_count
        LOGGER.info(f"Read boot count: {new_pdi.boot_count}")

        # Restore the old boot count
        self.node_link.pdi_write(PDI_ID.BOOT_COUNT, old_pdi)
        new_pdi = self.node_link.pdi_read(PDI_ID.BOOT_COUNT)
        assert isinstance(new_pdi, PDI_BootCount)
        assert new_pdi.boot_count == old_pdi.boot_count
        LOGGER.info(f"Restored boot count: {new_pdi.boot_count}")

    def test_voltage_configuration_pdi_data(self):
        """Test reading and writing voltage configuration PDI data."""
        fp_pdi_ids = [PDI_ID.TARGET_SYSTEM_VOLTAGE_OUTPUT]

        for pdi_id in fp_pdi_ids:
            # Read the current floating point PDI
            old_value = self.node_link.pdi_read(pdi_id)
            assert isinstance(old_value, PDI_FloatConfiguration)

            # Program a new value
            pgm_value = PDI_FloatConfiguration()
            pgm_value.value = random.uniform(-100.0, 100.0)
            assert self.node_link.pdi_write(pdi_id, pgm_value)

            # Read the new value
            new_value = self.node_link.pdi_read(pdi_id)
            assert isinstance(new_value, PDI_FloatConfiguration)
            assert new_value.value == pgm_value.value

            # Restore the old value
            self.node_link.pdi_write(pdi_id, old_value)
            new_value = self.node_link.pdi_read(pdi_id)
            assert isinstance(new_value, PDI_FloatConfiguration)
            assert new_value.value == old_value.value

    def test_current_configuration_pdi_data(self):
        """Test reading and writing voltage configuration PDI data."""
        fp_pdi_ids = [
            PDI_ID.TARGET_SYSTEM_CURRENT_OUTPUT,
            PDI_ID.CONFIG_SYSTEM_CURRENT_OUTPUT_RATED_LIMIT,
            PDI_ID.TARGET_PHASE_CURRENT_OUTPUT,
            PDI_ID.CONFIG_PHASE_CURRENT_OUTPUT_RATED_LIMIT,
        ]

        for pdi_id in fp_pdi_ids:
            # Read the current floating point PDI
            old_value = self.node_link.pdi_read(pdi_id)
            assert isinstance(old_value, PDI_FloatConfiguration)

            # Program a new value
            pgm_value = PDI_FloatConfiguration()
            pgm_value.value = random.uniform(-100.0, 100.0)
            assert self.node_link.pdi_write(pdi_id, pgm_value)

            # Read the new value
            new_value = self.node_link.pdi_read(pdi_id)
            assert isinstance(new_value, PDI_FloatConfiguration)
            assert new_value.value == pgm_value.value

            # Restore the old value
            self.node_link.pdi_write(pdi_id, old_value)
            new_value = self.node_link.pdi_read(pdi_id)
            assert isinstance(new_value, PDI_FloatConfiguration)
            assert new_value.value == old_value.value

    def test_filter_coefficients(self):
        """Test reading and writing filter coefficients."""
        test_pdi_ids = [
            PDI_ID.CONFIG_MON_FILTER_INPUT_VOLTAGE,
            PDI_ID.CONFIG_MON_FILTER_OUTPUT_VOLTAGE,
            PDI_ID.CONFIG_MON_FILTER_OUTPUT_CURRENT,
            PDI_ID.CONFIG_MON_FILTER_1V1_VOLTAGE,
            PDI_ID.CONFIG_MON_FILTER_3V3_VOLTAGE,
            PDI_ID.CONFIG_MON_FILTER_5V0_VOLTAGE,
            PDI_ID.CONFIG_MON_FILTER_12V0_VOLTAGE,
            PDI_ID.CONFIG_MON_FILTER_TEMPERATURE,
            PDI_ID.CONFIG_MON_FILTER_FAN_SPEED,
        ]

        for pdi_id in test_pdi_ids:
            # Read the existing filter coefficients
            old_value = self.node_link.pdi_read(pdi_id)
            assert isinstance(old_value, PDI_IIRFilterConfig)

            # Program a new value
            pgm_value = PDI_IIRFilterConfig()
            pgm_value.order = random.randint(1, 6)
            pgm_value.sampleRateMs = random.randint(10, 1000)
            pgm_value.coefficients.extend([random.uniform(-100.0, 100.0) for _ in range(15)])
            assert self.node_link.pdi_write(pdi_id, pgm_value)

            # Read the new value
            new_value = self.node_link.pdi_read(pdi_id)
            assert isinstance(new_value, PDI_IIRFilterConfig)
            assert new_value.order == pgm_value.order
            assert new_value.sampleRateMs == pgm_value.sampleRateMs
            assert new_value.coefficients == pgm_value.coefficients

            # Restore the old value
            assert self.node_link.pdi_write(pdi_id, old_value)
