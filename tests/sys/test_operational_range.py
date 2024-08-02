import logging
import time

import pytest

from ichnaea.ltc_7871 import REG_MFR_IDAC_VLOW, compute_optimal_vlow_idac
from tests.sys.fixtures import client
from loguru import logger

LOGGER = logging.getLogger(__name__)


@pytest.mark.parametrize("client", ["INFO"], indirect=True)
class TestOperationalRange:
    """ Operational range tests for the Ichnaea system. """

    node_under_test = '0xb04556ff'

    def test_single_idac_setting(self, client):
        """ Test the output voltage for a single IDAC setting. """
        test_voltages = [9.0, 12.0, 15.0]

        assert client.engage_output(self.node_under_test)
        assert client.write_ltc_register(self.node_under_test, REG_MFR_IDAC_VLOW, 0x00)
        time.sleep(1.0)

        for target_voltage in test_voltages:
            input_voltage = client.get_input_voltage(self.node_under_test)
            idac_value, ideal_output = compute_optimal_vlow_idac(input_voltage, target_voltage)

            assert client.write_ltc_register(self.node_under_test, REG_MFR_IDAC_VLOW, idac_value)
            time.sleep(1.0)

            actual_output = client.get_output_voltage(self.node_under_test)
            pct_error = abs((ideal_output - actual_output) / ideal_output) * 100
            logger.info(f"IDAC: {hex(idac_value)}, Target voltage: {target_voltage:.2f}, "
                        f"Ideal output: {ideal_output:.2f}, Actual output: {actual_output:.2f}, "
                        f"Error: {pct_error:.2f}%")

        assert client.write_ltc_register(self.node_under_test, REG_MFR_IDAC_VLOW, 0x00)
        assert client.disengage_output(self.node_under_test)

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
