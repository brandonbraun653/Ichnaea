import logging
import time

import numpy as np
import pytest_check as check
from loguru import logger

import ichnaea.ltc7871 as ltc
from ichnaea.board_values import ICHNAEA_OUTPUT_V_MIN
from tests.sys.fixtures import client

LOGGER = logging.getLogger(__name__)

node_under_test = '0xb04556ff'


# @pytest.mark.parametrize("client", ["INFO"], indirect=True)
class TestOperationalRange:
    """ Operational range tests for the Ichnaea system. """

    def test_voltage_sweep(self, client):
        """ Test the output voltage for a single IDAC setting. """
        assert client.engage_output(node_under_test)
        assert client.write_ltc_register(node_under_test, ltc.REG_MFR_IDAC_VLOW, 0x00)
        time.sleep(1.0)

        input_voltage = client.get_input_voltage(node_under_test)
        test_voltage_range = np.arange(ICHNAEA_OUTPUT_V_MIN, input_voltage, ltc.voltage_resolution())
        LOGGER.info(f"Testing voltage controllability over the range: "
                    f"{input_voltage:.2f}V -> {ICHNAEA_OUTPUT_V_MIN:.2f}V in {len(test_voltage_range)} steps")

        for target in test_voltage_range:
            idac_value, ideal_output = ltc.compute_optimal_vlow_idac(target)

            assert client.write_ltc_register(node_under_test, ltc.REG_MFR_IDAC_VLOW, idac_value)
            time.sleep(1.0)

            actual_output = client.get_output_voltage(node_under_test)
            assert actual_output is not None

            pct_error = abs((ideal_output - actual_output) / ideal_output)
            LOGGER.info(f"IDAC: {hex(idac_value)}, Actual: {actual_output:.2f}")
            check.is_true(pct_error <= ltc.PWR_GOOD_TOLERANCE, f"IDAC: {hex(idac_value)}, "
                                                               f"Target: {target:.2f}, "
                                                               f"Expected: {ideal_output:.2f}, "
                                                               f"Actual: {actual_output:.2f}, "
                                                               f"Error: {pct_error:.2f}%")

        assert client.write_ltc_register(node_under_test, ltc.REG_MFR_IDAC_VLOW, 0x00)
        assert client.disengage_output(node_under_test)
