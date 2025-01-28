import logging
import operator

from ichnaea.node_client import MIN_INPUT_VOLTAGE
from ichnaea.proto.ichnaea_pdi_pb2 import *
from ichnaea.proto.ichnaea_rpc_pb2 import SensorType
from tests.sys.fixtures import *

LOGGER = logging.getLogger(__name__)


@pytest.mark.parametrize("product_config", ["hardware"], indirect=True)
class TestSandbox:
    """Human in the loop playground testing with the hardware platform"""

    TestParam_MinInputVoltage = 20.0  # Minimum input voltage this test requires

    @pytest.fixture(autouse=True)
    def setup_method(self, node_link: NodeClient, product_config):
        """Common setup routines before each test case"""
        self.node_link: NodeClient = node_link
        assert product_config["type"] == Platform.Hardware, "Test only valid for hardware"

        # Check preconditions for the test
        input_voltage = self.node_link.await_sensor_value(
            SensorType.SENSOR_INPUT_VOLTAGE,
            target=self.TestParam_MinInputVoltage,
            comparison=operator.ge,
            tolerance=None,
        )
        assert input_voltage >= self.TestParam_MinInputVoltage, "Input voltage too low"

        # Ensure we start out in a known state
        self.node_link.disengage_output()

        # Program operational limits for the device
        assert self.node_link.pdi_write(
            PDI_ID.CONFIG_MIN_SYSTEM_VOLTAGE_INPUT_RATED_LIMIT, PDI_FloatConfiguration(value=10.0)
        )
        assert self.node_link.pdi_write(
            PDI_ID.CONFIG_MAX_SYSTEM_VOLTAGE_INPUT_RATED_LIMIT, PDI_FloatConfiguration(value=100.0)
        )
        assert self.node_link.pdi_write(
            PDI_ID.CONFIG_SYSTEM_VOLTAGE_OUTPUT_RATED_LIMIT, PDI_FloatConfiguration(value=60.0)
        )
        assert self.node_link.pdi_write(
            PDI_ID.CONFIG_SYSTEM_CURRENT_OUTPUT_RATED_LIMIT, PDI_FloatConfiguration(value=150.0)
        )
        assert self.node_link.pdi_write(
            PDI_ID.CONFIG_PHASE_CURRENT_OUTPUT_RATED_LIMIT, PDI_FloatConfiguration(value=28.0)
        )

        # Program user defined operational limits for the device
        assert self.node_link.pdi_write(PDI_ID.TARGET_SYSTEM_CURRENT_OUTPUT, PDI_FloatConfiguration(value=25.0))

    def teardown_method(self) -> None:
        """Common teardown routines after each test case"""
        self.node_link.disengage_output()

    def test_simple_power_on(self):
        """Power on Ichnaea, then read out all status information"""
        # Program the desired settings
        input_voltage = self.node_link.await_sensor_value(
            SensorType.SENSOR_INPUT_VOLTAGE, target=MIN_INPUT_VOLTAGE, comparison=operator.ge, tolerance=None
        )
        assert input_voltage, "Input voltage target not met"

        # Set the output voltage and current limits
        assert self.node_link.set_output_voltage_target(12.0), "Unable to set output voltage"
        assert self.node_link.set_output_current_target(10.0), "Unable to set output current limit"

        # Engage the output, wait for the system to enable and stabilize
        assert self.node_link.engage_output()
        assert self.node_link.await_sensor_value(SensorType.SENSOR_OUTPUT_VOLTAGE, target=12.0, tolerance=0.1)
        self.node_link.sleep_on_node_time(2.0)

        # Disengage the output and ensure the voltage drops (safety)
        assert self.node_link.disengage_output()
        assert (
            self.node_link.await_sensor_value(
                SensorType.SENSOR_OUTPUT_VOLTAGE, target=5.0, comparison=operator.le, tolerance=None
            )
            is not None
        )

    def test_calibrate_sensors(self):
        """Calibrate the system sensors"""
        assert self.node_link.zero_load_current_offset()
        # self.node_link.reboot()
