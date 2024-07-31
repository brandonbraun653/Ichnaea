import logging
import time

import pytest
from tests.sys.fixtures import client
from loguru import logger

LOGGER = logging.getLogger(__name__)


@pytest.mark.parametrize("client", ["INFO"], indirect=True)
class TestOperationalRange:
    """ Operational range tests for the Ichnaea system. """

    node_under_test = '0xb04556ff'
    reg_mfr_idac_vlow = 0x08

    def test_voltage_sweep(self, client):
        """ Test the operational range of the output voltage by sweeping the IDAC register values. """
        logger.info("Testing the operational range of the output voltage by sweeping the IDAC register values")
        input_voltage = client.get_input_voltage(self.node_under_test)
        logger.info(f"Input voltage: {input_voltage}")

        assert client.engage_output(self.node_under_test)

        for idac in range(0, 255, 5):
            assert client.write_ltc_register(self.node_under_test, self.reg_mfr_idac_vlow, idac)
            time.sleep(0.5)
            input_voltage = client.get_input_voltage(self.node_under_test)
            output_voltage = client.get_output_voltage(self.node_under_test)
            logger.info(f"IDAC: {hex(idac)}, Output voltage: {output_voltage:.2f}, Input voltage: {input_voltage:.2f}")

        # TODO: Later on you could tabulate this data and validate it against some expected percent accessible
        #  operational range. Might be useful to also produce a graph of the data for customers.
