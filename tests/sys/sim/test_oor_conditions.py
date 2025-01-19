import logging
import operator

from ichnaea.node_client import MIN_INPUT_VOLTAGE
from ichnaea.proto.ichnaea_pdi_pb2 import *
from ichnaea.proto.ichnaea_rpc_pb2 import SensorType, EngageState
from ichnaea.simulator.grpc_client import EnvironmentSpoofer
from tests.sys.fixtures import *

LOGGER = logging.getLogger(__name__)


@pytest.mark.parametrize("product_config", ["simulator"], indirect=True)
class TestOutofRangeConditions:
    """Test destructive or hazardous conditions that may occur during operation."""

    TestParam_MinInputVoltage = 24.0  # Minimum input voltage this test requires

    @pytest.fixture(autouse=True)
    def setup_method(self, node_link: NodeClient, product_config):
        """Common setup routines before each test case"""
        self.node_link: NodeClient = node_link
        self.platform: Platform = product_config["type"]

        # Prevent spurrious temporal issues with logging by erasing the log before each test
        assert self.node_link.log_erase()

        # Check testing environment conditions to ensure we're ready to run
        if self.platform == Platform.Simulator:
            self.env = EnvironmentSpoofer()
            self.env.disconnect_load()
            self.env.set_solar_ocv(48.0)
            self.env.set_board_1v1_rail(1.1)
            self.env.set_board_3v3_rail(3.3)
            self.env.set_board_5v_rail(5.0)
            self.env.set_board_12v_rail(12.0)
            self.env.set_board_temperature(25.0)
            self.env.set_board_fan_speed(200.0)

        # Check preconditions for the test
        input_voltage = self.node_link.await_sensor_value(
            SensorType.SENSOR_INPUT_VOLTAGE,
            target=self.TestParam_MinInputVoltage,
            comparison=operator.ge,
            tolerance=None,
        )
        assert input_voltage >= self.TestParam_MinInputVoltage, "Input voltage too low"

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
        # assert self.node_link.pdi_write(PDI_ID.CONFIG_MIN_SYSTEM_VOLTAGE_INPUT, PDI_FloatConfiguration(value=15.0))
        # assert self.node_link.pdi_write(PDI_ID.CONFIG_MAX_SYSTEM_VOLTAGE_INPUT, PDI_FloatConfiguration(value=80.0))

    def teardown_method(self) -> None:
        """Common teardown routines after each test case"""
        self.env.disconnect_load()
        self.node_link.disengage_output()

    def test_simple_power_on(self):
        """Power on Ichnaea, then read out all status information"""
        # Program the desired settings
        input_voltage = self.node_link.await_sensor_value(
            SensorType.SENSOR_INPUT_VOLTAGE, target=MIN_INPUT_VOLTAGE, comparison=operator.ge, tolerance=None
        )
        assert input_voltage, "Input voltage target not met"

        voltage_target = max(input_voltage / 2.0, 12.0)
        ilim_target = 1.0

        assert self.node_link.set_output_voltage_target(voltage_target), "Unable to set output voltage"
        assert self.node_link.set_output_current_target(ilim_target), "Unable to set output current limit"

        # Engage the output, wait for the system to enable and stabilize
        assert self.node_link.engage_output()
        assert self.node_link.await_sensor_value(SensorType.SENSOR_OUTPUT_VOLTAGE, target=voltage_target)

        # Apply a simple load to the system to get non-zero current measurements
        if self.platform == Platform.Simulator:
            r_load = 48  # Ohms
            expected_current = voltage_target / r_load
            self.env.apply_load(r_load)
            assert self.node_link.await_sensor_value(SensorType.SENSOR_OUTPUT_CURRENT, target=expected_current)

        # Disengage the output and ensure the voltage drops (safety)
        assert self.node_link.disengage_output()
        assert (
            self.node_link.await_sensor_value(
                SensorType.SENSOR_OUTPUT_VOLTAGE, target=5.0, comparison=operator.le, tolerance=None
            )
            is not None
        )

    def test_12v_out_of_range_low(self):
        """Inject an OOR voltage and ensure the system shuts down"""
        if not self.platform == Platform.Simulator:
            pytest.skip("Test requires simulator")

        # Engage the output, wait for the system to enable and stabilize
        target_voltage = 15.0
        assert self.node_link.set_output_voltage_target(target_voltage), "Unable to set output voltage"
        assert self.node_link.engage_output()
        assert self.node_link.await_sensor_value(SensorType.SENSOR_OUTPUT_VOLTAGE, target=target_voltage)
        assert self.node_link.engagement_state() == EngageState.ENGAGED

        # Drive a 12v rail OOR condition
        self.env.set_board_12v_rail(10.0)
        self.node_link.sleep_on_node_time(1.0)

        # Check the expectation that the system will shut down
        assert self.node_link.engagement_state() == EngageState.DISENGAGED

    def test_12v_out_of_range_high(self):
        """Inject an OOR voltage and ensure the system shuts down"""
        if not self.platform == Platform.Simulator:
            pytest.skip("Test requires simulator")

        # Engage the output, wait for the system to enable and stabilize
        target_voltage = 15.0
        assert self.node_link.set_output_voltage_target(target_voltage), "Unable to set output voltage"
        assert self.node_link.engage_output()
        assert self.node_link.await_sensor_value(SensorType.SENSOR_OUTPUT_VOLTAGE, target=target_voltage)
        assert self.node_link.engagement_state() == EngageState.ENGAGED

        # Drive a 12v rail OOR condition
        self.env.set_board_12v_rail(15.1)
        self.node_link.sleep_on_node_time(1.0)

        # Check the expectation that the system will shut down
        assert self.node_link.engagement_state() == EngageState.DISENGAGED

    def test_vin_out_of_range_low(self):
        """Inject an OOR voltage and ensure the system shuts down"""
        if not self.platform == Platform.Simulator:
            pytest.skip("Test requires simulator")

        # Engage the output, wait for the system to enable and stabilize
        target_voltage = 15.0
        assert self.node_link.set_output_voltage_target(target_voltage), "Unable to set output voltage"
        assert self.node_link.engage_output()
        assert self.node_link.await_sensor_value(SensorType.SENSOR_OUTPUT_VOLTAGE, target=target_voltage)
        assert self.node_link.engagement_state() == EngageState.ENGAGED

        # Drive a VIN OOR condition
        pdi_vin_limit_min = self.node_link.pdi_read(PDI_ID.CONFIG_MIN_SYSTEM_VOLTAGE_INPUT_RATED_LIMIT)
        assert isinstance(pdi_vin_limit_min, PDI_FloatConfiguration)

        test_input_voltage = max(pdi_vin_limit_min.value - 1.0, 0.0)
        self.env.set_solar_ocv(test_input_voltage)
        self.node_link.sleep_on_node_time(1.0)

        # Check the expectation that the system will shut down
        assert self.node_link.engagement_state() == EngageState.DISENGAGED

    def test_vin_out_of_range_high(self):
        """Inject an OOR voltage and ensure the system shuts down"""
        if not self.platform == Platform.Simulator:
            pytest.skip("Test requires simulator")

        # Engage the output, wait for the system to enable and stabilize
        target_voltage = 15.0
        assert self.node_link.set_output_voltage_target(target_voltage), "Unable to set output voltage"
        assert self.node_link.engage_output()
        assert self.node_link.await_sensor_value(SensorType.SENSOR_OUTPUT_VOLTAGE, target=target_voltage)
        assert self.node_link.engagement_state() == EngageState.ENGAGED

        # Drive a VIN OOR condition
        pdi_vin_limit_max = self.node_link.pdi_read(PDI_ID.CONFIG_MAX_SYSTEM_VOLTAGE_INPUT_RATED_LIMIT)
        assert isinstance(pdi_vin_limit_max, PDI_FloatConfiguration)

        test_input_voltage = pdi_vin_limit_max.value + 1.0
        self.env.set_solar_ocv(test_input_voltage)
        self.node_link.sleep_on_node_time(0.5)

        # Check the expectation that the system will shut down
        assert self.node_link.engagement_state() == EngageState.DISENGAGED

    def test_iout_system_limit_exceeded(self):
        """Inject +5% over-current system rated limit condition and ensure the system shuts down"""
        if not self.platform == Platform.Simulator:
            pytest.skip("Test requires simulator")

        # Engage the output, wait for the system to enable and stabilize
        target_voltage = 45.0
        assert self.node_link.set_output_voltage_target(target_voltage), "Unable to set output voltage"
        assert self.node_link.engage_output()
        assert self.node_link.await_sensor_value(SensorType.SENSOR_OUTPUT_VOLTAGE, target=target_voltage)
        assert self.node_link.engagement_state() == EngageState.ENGAGED

        # Drive +5% over-current condition
        pdi_ilim_limit = self.node_link.pdi_read(PDI_ID.CONFIG_SYSTEM_CURRENT_OUTPUT_RATED_LIMIT)
        assert isinstance(pdi_ilim_limit, PDI_FloatConfiguration)
        test_load = target_voltage / (pdi_ilim_limit.value * 1.05)
        self.env.apply_load(resistance=test_load)
        self.node_link.sleep_on_node_time(0.5)

        # Check the expectation that the system will shut down
        assert self.node_link.engagement_state() == EngageState.DISENGAGED

    def test_iout_target_out_of_range_high(self):
        """Inject +5% over-current user defined limit condition and ensure the system shuts down"""
        if not self.platform == Platform.Simulator:
            pytest.skip("Test requires simulator")

        # Program the test parameters
        assert self.node_link.pdi_write(PDI_ID.TARGET_SYSTEM_CURRENT_OUTPUT, PDI_FloatConfiguration(value=25.0))
        assert self.node_link.pdi_write(
            PDI_ID.CONFIG_SYSTEM_CURRENT_OUTPUT_RATED_LIMIT, PDI_FloatConfiguration(value=150.0)
        )

        # Engage the output, wait for the system to enable and stabilize
        target_voltage = 45.0
        assert self.node_link.set_output_voltage_target(target_voltage), "Unable to set output voltage"
        assert self.node_link.engage_output()
        assert self.node_link.await_sensor_value(SensorType.SENSOR_OUTPUT_VOLTAGE, target=target_voltage)
        assert self.node_link.engagement_state() == EngageState.ENGAGED

        # Drive +5% over-current condition
        pdi_ilim_limit = self.node_link.pdi_read(PDI_ID.TARGET_SYSTEM_CURRENT_OUTPUT)
        assert isinstance(pdi_ilim_limit, PDI_FloatConfiguration)
        test_load = target_voltage / (pdi_ilim_limit.value * 1.05)
        self.env.apply_load(resistance=test_load)
        self.node_link.sleep_on_node_time(0.5)

        # Check the expectation that the system will shut down
        assert self.node_link.engagement_state() == EngageState.DISENGAGED

    def test_vout_system_limit_exceeded(self):
        """Inject an OOR voltage and ensure the system shuts down"""
        if not self.platform == Platform.Simulator:
            pytest.skip("Test requires simulator")

        # Engage the output, wait for the system to enable and stabilize
        target_voltage = 23.5
        assert self.node_link.set_output_voltage_target(target_voltage), "Unable to set output voltage"
        assert self.node_link.engage_output()
        assert self.node_link.engagement_state() == EngageState.ENGAGED
        assert self.node_link.await_sensor_value(SensorType.SENSOR_OUTPUT_VOLTAGE, target=target_voltage)

        # Get the current output voltage limit
        pdi_vout_limit_max = self.node_link.pdi_read(PDI_ID.CONFIG_SYSTEM_VOLTAGE_OUTPUT_RATED_LIMIT)
        assert isinstance(pdi_vout_limit_max, PDI_FloatConfiguration)

        # Set the output voltage to be almost out of range
        test_output_voltage = max(pdi_vout_limit_max.value - 1.0, 0.0)
        self.node_link.set_output_voltage_target(test_output_voltage)
        assert self.node_link.await_sensor_value(SensorType.SENSOR_OUTPUT_VOLTAGE, target=test_output_voltage)

        # Drop the rated limit below the voltage target, causing the monitor to fault
        new_rated_limit = max(test_output_voltage - 5.0, 0.0)
        assert self.node_link.pdi_write(
            PDI_ID.CONFIG_SYSTEM_VOLTAGE_OUTPUT_RATED_LIMIT, PDI_FloatConfiguration(value=new_rated_limit)
        )
        self.node_link.sleep_on_node_time(1.5)

        # Check the expectation that the system will shut down
        assert self.node_link.engagement_state() == EngageState.DISENGAGED

        # Program back the old rated limit
        assert self.node_link.pdi_write(
            PDI_ID.CONFIG_SYSTEM_VOLTAGE_OUTPUT_RATED_LIMIT, PDI_FloatConfiguration(value=pdi_vout_limit_max.value)
        )

    def test_vout_out_of_range_low(self):
        """Inject an OOR voltage and ensure the system shuts down"""
        if not self.platform == Platform.Simulator:
            pytest.skip("Test requires simulator")

        # Engage the output, wait for the system to enable and stabilize
        target_voltage = 23.5
        assert self.node_link.set_output_voltage_target(target_voltage), "Unable to set output voltage"
        assert self.node_link.engage_output()
        assert self.node_link.engagement_state() == EngageState.ENGAGED
        assert self.node_link.await_sensor_value(SensorType.SENSOR_OUTPUT_VOLTAGE, target=target_voltage)

        # Simulate another system being connected that has a lower output voltage.
        self.env.set_output_voltage(10.0)
        self.node_link.sleep_on_node_time(1.5)

        # Check the expectation that the system will shut down
        assert self.node_link.engagement_state() == EngageState.DISENGAGED

    def test_vout_out_of_range_high(self):
        """Inject an OOR voltage and ensure the system shuts down"""
        if not self.platform == Platform.Simulator:
            pytest.skip("Test requires simulator")

        # Engage the output, wait for the system to enable and stabilize
        target_voltage = 23.5
        assert self.node_link.set_output_voltage_target(target_voltage), "Unable to set output voltage"
        assert self.node_link.engage_output()
        assert self.node_link.engagement_state() == EngageState.ENGAGED
        assert self.node_link.await_sensor_value(SensorType.SENSOR_OUTPUT_VOLTAGE, target=target_voltage)

        # Simulate another system being connected that has a higher output voltage.
        self.env.set_output_voltage(30.0)
        self.node_link.sleep_on_node_time(1.5)

        # Check the expectation that the system will shut down
        assert self.node_link.engagement_state() == EngageState.DISENGAGED

    def test_temperature_out_of_range_low(self):
        """Inject an OOR temperature and ensure the system shuts down"""
        if not self.platform == Platform.Simulator:
            pytest.skip("Test requires simulator")

        # Engage the output, wait for the system to enable and stabilize
        target_voltage = 23.5
        assert self.node_link.set_output_voltage_target(target_voltage), "Unable to set output voltage"
        assert self.node_link.engage_output()
        assert self.node_link.engagement_state() == EngageState.ENGAGED
        assert self.node_link.await_sensor_value(SensorType.SENSOR_OUTPUT_VOLTAGE, target=target_voltage)

        # Program the system limits
        assert self.node_link.pdi_write(PDI_ID.CONFIG_MIN_TEMP_LIMIT, PDI_FloatConfiguration(value=10.0))
        assert self.node_link.pdi_write(PDI_ID.CONFIG_MAX_TEMP_LIMIT, PDI_FloatConfiguration(value=70.0))

        # Simulate an OOR temperature condition
        self.env.set_board_temperature(5.0)
        self.node_link.sleep_on_node_time(1.5)

        # Check the expectation that the system will shut down
        assert self.node_link.engagement_state() == EngageState.DISENGAGED

    def test_temperature_out_of_range_high(self):
        """Inject an OOR temperature and ensure the system shuts down"""
        if not self.platform == Platform.Simulator:
            pytest.skip("Test requires simulator")

        # Engage the output, wait for the system to enable and stabilize
        target_voltage = 23.5
        assert self.node_link.set_output_voltage_target(target_voltage), "Unable to set output voltage"
        assert self.node_link.engage_output()
        assert self.node_link.engagement_state() == EngageState.ENGAGED
        assert self.node_link.await_sensor_value(SensorType.SENSOR_OUTPUT_VOLTAGE, target=target_voltage)

        # Program the system limits
        assert self.node_link.pdi_write(PDI_ID.CONFIG_MIN_TEMP_LIMIT, PDI_FloatConfiguration(value=10.0))
        assert self.node_link.pdi_write(PDI_ID.CONFIG_MAX_TEMP_LIMIT, PDI_FloatConfiguration(value=70.0))

        # Simulate an OOR temperature condition
        self.env.set_board_temperature(75.0)
        self.node_link.sleep_on_node_time(1.5)

        # Check the expectation that the system will shut down
        assert self.node_link.engagement_state() == EngageState.DISENGAGED

    def test_fan_speed_out_of_range_low(self):
        """Inject an OOR fan speed and ensure the system shuts down"""
        if not self.platform == Platform.Simulator:
            pytest.skip("Test requires simulator")

        # Engage the output, wait for the system to enable and stabilize
        target_voltage = 23.5
        assert self.node_link.set_output_voltage_target(target_voltage), "Unable to set output voltage"
        assert self.node_link.engage_output()
        assert self.node_link.engagement_state() == EngageState.ENGAGED
        assert self.node_link.await_sensor_value(SensorType.SENSOR_OUTPUT_VOLTAGE, target=target_voltage)

        # Program the system limits
        target_speed = 500.0
        oor_timeout_ms = 1500
        assert self.node_link.pdi_write(
            PDI_ID.CONFIG_MON_FAN_SPEED_PCT_ERROR_OOR_LIMIT, PDI_FloatConfiguration(value=0.1)
        )
        assert self.node_link.pdi_write(PDI_ID.TARGET_FAN_SPEED_RPM, PDI_FloatConfiguration(value=target_speed))
        assert self.node_link.pdi_write(
            PDI_ID.CONFIG_MON_FAN_SPEED_OOR_ENTRY_DELAY_MS, PDI_Uint32Configuration(value=oor_timeout_ms)
        )

        # Simulate an OOR fan speed condition
        self.env.set_board_fan_speed(target_speed - (target_speed * 0.2))

        # Check the expectation that the system will shut down
        assert self.node_link.wait_for_engagement_state(target=EngageState.DISENGAGED, timeout=25)

    def test_fan_speed_out_of_range_high(self):
        """Inject an OOR fan speed and ensure the system shuts down"""
        if not self.platform == Platform.Simulator:
            pytest.skip("Test requires simulator")

        # Engage the output, wait for the system to enable and stabilize
        target_voltage = 23.5
        assert self.node_link.set_output_voltage_target(target_voltage), "Unable to set output voltage"
        assert self.node_link.engage_output()
        assert self.node_link.engagement_state() == EngageState.ENGAGED
        assert self.node_link.await_sensor_value(SensorType.SENSOR_OUTPUT_VOLTAGE, target=target_voltage)

        # Program the system limits
        target_speed = 500.0
        oor_timeout_ms = 1500
        assert self.node_link.pdi_write(
            PDI_ID.CONFIG_MON_FAN_SPEED_PCT_ERROR_OOR_LIMIT, PDI_FloatConfiguration(value=0.1)
        )
        assert self.node_link.pdi_write(PDI_ID.TARGET_FAN_SPEED_RPM, PDI_FloatConfiguration(value=target_speed))
        assert self.node_link.pdi_write(
            PDI_ID.CONFIG_MON_FAN_SPEED_OOR_ENTRY_DELAY_MS, PDI_Uint32Configuration(value=oor_timeout_ms)
        )

        # Simulate an OOR fan speed condition
        self.env.set_board_fan_speed(target_speed + (target_speed * 0.2))

        # Check the expectation that the system will shut down
        assert self.node_link.wait_for_engagement_state(target=EngageState.DISENGAGED, timeout=25)
