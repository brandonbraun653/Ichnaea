import time
import operator
from typing import Callable, List
from typing import Optional

from loguru import logger

from ichnaea.network_client import NetworkClient
from ichnaea.messages import HeartbeatPBMsg
from ichnaea.proto.ichnaea_pdi_pb2 import *
from ichnaea.proto.ichnaea_rpc_pb2 import *
from mbedutils.rpc.logger_client import LoggerRPCClient
from mbedutils.rpc.message import BasePBMsg
from mbedutils.rpc.observer_impl import PredicateObserver
from mbedutils.rpc.proto.mbed_rpc_pb2 import LoggerWriteRequest

# Map of PDI IDs to their corresponding protobuf message types
pdi_type_to_id_map = {
    PDI_FloatConfiguration: [
        PDI_ID.TARGET_SYSTEM_CURRENT_OUTPUT,
        PDI_ID.CONFIG_SYSTEM_CURRENT_OUTPUT_RATED_LIMIT,
        PDI_ID.TARGET_PHASE_CURRENT_OUTPUT,
        PDI_ID.CONFIG_PHASE_CURRENT_OUTPUT_RATED_LIMIT,
        PDI_ID.TARGET_SYSTEM_VOLTAGE_OUTPUT,
        PDI_ID.CONFIG_SYSTEM_VOLTAGE_OUTPUT_RATED_LIMIT,
        PDI_ID.CONFIG_MIN_SYSTEM_VOLTAGE_INPUT,
        PDI_ID.CONFIG_MIN_SYSTEM_VOLTAGE_INPUT_RATED_LIMIT,
        PDI_ID.CONFIG_MAX_SYSTEM_VOLTAGE_INPUT,
        PDI_ID.CONFIG_MAX_SYSTEM_VOLTAGE_INPUT_RATED_LIMIT,
    ],
    PDI_IIRFilterConfig: [
        PDI_ID.CONFIG_MON_FILTER_INPUT_VOLTAGE,
        PDI_ID.CONFIG_MON_FILTER_OUTPUT_VOLTAGE,
        PDI_ID.CONFIG_MON_FILTER_OUTPUT_CURRENT,
        PDI_ID.CONFIG_MON_FILTER_1V1_VOLTAGE,
        PDI_ID.CONFIG_MON_FILTER_3V3_VOLTAGE,
        PDI_ID.CONFIG_MON_FILTER_5V0_VOLTAGE,
        PDI_ID.CONFIG_MON_FILTER_12V0_VOLTAGE,
        PDI_ID.CONFIG_MON_FILTER_TEMPERATURE,
        PDI_ID.CONFIG_MON_FILTER_FAN_SPEED,
    ],
}

# Map of PDI IDs to their corresponding protobuf message types. Single PDI types are
# listed explicitly here, while multiple PDI ids mapped to the same type are listed above
# and then added to this map in the loop below.
pdi_id_type_map = {
    PDI_ID.BOOT_COUNT: PDI_BootCount,
}

# Update the pdi_id_type_map with the reverse mapping from the pdi_type_to_id_map
for k, v in pdi_type_to_id_map.items():
    for pdi_id in v:
        pdi_id_type_map[pdi_id] = k

MIN_INPUT_VOLTAGE = 15.0
MAX_INPUT_VOLTAGE = 100.0
MAX_OUTPUT_VOLTAGE = 55.0
MIN_OUTPUT_VOLTAGE = 10.0
MAX_OUTPUT_CURRENT = 150.0


class NodeClient:
    """Interface to a single Ichnaea node in the distributed network"""

    def __init__(self, node_id: str, client: NetworkClient):
        """
        Args:
            node_id: The unique identifier of the node
            client: The client object to use for communication
        """
        self._node_id = node_id
        self._net_client = client

        # Internal configuration data about this node
        self._rated_system_output_voltage = MAX_OUTPUT_VOLTAGE
        self._rated_system_output_current = MAX_OUTPUT_CURRENT

    @property
    def node_id(self) -> str:
        return self._node_id

    def is_alive(self) -> bool:
        """
        Checks if the node is alive via heartbeat reporting
        Returns:
            True if the node is alive, False if not
        """
        return self._net_client.is_alive(self._node_id)

    def sleep_on_node_time(self, seconds: float) -> None:
        """
        Sleeps the current thread for at least a specified number of seconds, but does so in a way that
        synchronizes with the node's internal clock. This is useful for coordinating time-sensitive
        operations with the node.

        Args:
            seconds: Number of seconds to sleep for

        Raises:
            RuntimeWarning: If the node does not respond to the sleep request

        Returns:
            None
        """
        self._net_client.rpc_client.sleep_on_server_time_ms(int(seconds * 1000))

    def pdi_read(self, pdi: int) -> Optional[Message]:
        """
        Reads a PDI value from the node
        Args:
            pdi: The ID of the PDI to read

        Returns:
            The PDI value encoded as a protobuf message, or None if the PDI is not present
            or not readable.
        """
        pdi_type = pdi_id_type_map.get(pdi)
        if pdi_type is None:
            raise ValueError(f"Unmapped PDI ID: {pdi}")

        pdi_data = self._net_client.pdi_read(self._node_id, pdi)
        if pdi_data:
            pdi = pdi_type()
            pdi.ParseFromString(pdi_data)
            return pdi
        else:
            return None

    def pdi_write(self, pdi: int, msg: Message) -> bool:
        """
        Writes a PDI value to the node
        Args:
            pdi: The ID of the PDI to write
            msg: The PDI value to write, as a protobuf message

        Returns:
            True if the write was successfully committed, False if not
        """
        if self._net_client.pdi_write(self._node_id, pdi, msg.SerializeToString()):
            return self.pdi_read(pdi) == msg
        return False

    def pdi_flush(self) -> None:
        """
        Flushes the PDI cache on the node to ensure all data is written to non-volatile memory. This is
        a barrier operation and may take some time to complete.

        Returns:
            None
        """
        if not self._net_client.send_command(self._node_id, CMD_FLUSH_PDI_CACHE, timeout=10.0):
            logger.error(f"Failed to flush PDI cache on {self._node_id}")

    def zero_load_current_offset(self) -> bool:
        """
        Instructs the node to zero the load current offset. This is useful for calibrating the current
        sensor in the power stage.

        Returns:
            True if the operation succeeded, False if not
        """
        return self._net_client.send_command(self._node_id, CMD_ZERO_OUTPUT_CURRENT, timeout=3.0)

    def log_erase(self) -> bool:
        """
        Erases the log memory on the node
        Returns:
            True if the operation succeeded, False if not
        """
        # TODO BMB: This is assuming a direct connection to the node. Needs updates for meshed network.
        log_client = LoggerRPCClient(rpc_client=self._net_client.rpc_client, logger_id=0)
        return log_client.erase()

    def log_write(self, message: str, level: LoggerWriteRequest.Level) -> bool:
        """
        Writes a message to the log on the node
        Args:
            message: The message to write to the log

        Returns:
            True if the operation succeeded, False if not
        """
        # TODO BMB: This is assuming a direct connection to the node. Needs updates for meshed network.
        log_client = LoggerRPCClient(rpc_client=self._net_client.rpc_client, logger_id=0)
        data_to_send = message.encode("utf-8")
        return log_client.write(data_to_send, level=level)

    def log_read(self, count: int = 1, direction: bool = True) -> List[str]:
        """
        Reads a number of logs from the node
        Args:
            count: The number of log entries to read
            direction: The direction to read the logs, True for newest to oldest, False for oldest to newest

        Returns:
            A list of log entries, empty if no logs are present
        """
        # TODO BMB: This is assuming a direct connection to the node. Needs updates for meshed network.
        log_client = LoggerRPCClient(rpc_client=self._net_client.rpc_client, logger_id=0)
        return [entry.decode("utf-8") for entry in log_client.read(count, direction)]

    def reboot(self, timeout: float = 5.0) -> None:
        """
        Sends a command to reboot the node.
        Args:
            timeout: How long to wait for the reboot to complete

        Returns:
            None
        """
        # Check if the node is already alive before sending the reboot command
        if not self.is_alive():
            logger.warning(f"Node {self._node_id} is not alive, skipping reboot")
            return

        # Cache the old boot count to compare against the new one
        hb = self._net_client.get_last_heartbeat(self._node_id)
        old_boot_count = hb.pb_message.boot_count

        # Construct an observer that listens for the new boot count to increment from the heartbeat
        def _reboot_predicate_observer(msg: BasePBMsg) -> bool:
            if isinstance(msg, HeartbeatPBMsg):
                msg_node_id = self._net_client.unique_id_to_string(msg.pb_message.node_id)
                return (msg.pb_message.boot_count > old_boot_count) and (msg_node_id == self._node_id)
            return False

        reboot_observer = PredicateObserver(_reboot_predicate_observer, timeout=timeout)
        observer_id = self._net_client.rpc_client.com_pipe.subscribe_observer(reboot_observer)

        # Send a non-blocking reboot command
        self._net_client.send_command(self._node_id, CMD_REBOOT, timeout=0)

        # Wait for the reboot to complete or timeout
        matched_msg = reboot_observer.wait()
        self._net_client.rpc_client.com_pipe.unsubscribe(observer_id)

        if not matched_msg:
            logger.warning(f"Reboot of node {self._node_id} timed out")

    def engage_output(self) -> bool:
        """
        Instructs a node to power on its output stage. This requires the node to perform a safety check
        before engaging and may take some time to complete. Uses pre-programmed settings for the output
        stage.

        Returns:
            True if the operation succeeded, False if not
        """
        if not self._net_client.send_command(self._node_id, CMD_ENGAGE_OUTPUT, timeout=2.0):
            logger.error(f"Failed to engage output on {self._node_id}")
            return False
        return True

    def disengage_output(self) -> bool:
        """
        Instructs a node to power off the output stage, preventing power from reaching the load.

        Returns:
            True if the operation succeeded, False if not
        """
        if not self._net_client.send_command(self._node_id, CMD_DISENGAGE_OUTPUT, timeout=2.0):
            logger.error(f"Failed to disengage output on {self._node_id}")
            return False
        return True

    def engagement_state(self) -> Optional[EngageState.ValueType]:
        """
        Gets the power stage engagement status of the node
        Returns:
            State of the power stage
        """
        status = self._net_client.get_status(self._node_id)
        if status is not None:
            return status.pb_message.output_state

        logger.warning(f"{self._node_id} did not respond to status request")
        return None

    def wait_for_engagement_state(
        self, target: EngageState.ValueType, timeout: float = 5.0, poll_rate: float = 0.5
    ) -> bool:
        """
        Waits for the node to reach a target engagement state
        Args:
            target_state: The target state to wait for
            timeout: How long to wait for the node to reach the target state
            poll_rate: How often to poll the node for its current state

        Returns:
            True if reached a state that matched the target, False if timed out or the node did not respond
        """
        # Inspect the node for state changes
        poll_rate = max(0.1, poll_rate)
        start_time = time.time()
        actual = None
        while (time.time() - start_time) < timeout:
            actual = self.engagement_state()
            if actual == target:
                return True
            time.sleep(poll_rate)

        # Timed out!
        if actual:
            logger.warning(f"Engagement state {actual} did not meet target {target}")
            return False

        # Node did not respond
        return False

    def set_output_voltage_target(self, target: float) -> bool:
        """
        Sets the output voltage target for a node. This does not engage the output stage, only sets the
        desired voltage level for the power stage to achieve.

        Args:
            target: The desired output voltage in volts

        Returns:
            True if the operation succeeded, False if not
        """
        assert 0.0 <= target, f"Output voltage out of range: {target:.2f} V"
        return self.pdi_write(PDI_ID.TARGET_SYSTEM_VOLTAGE_OUTPUT, PDI_FloatConfiguration(value=target))

    def get_output_voltage_target(self) -> Optional[float]:
        """
        Gets the target output voltage setting from the node
        Returns:
            Output voltage in volts
        """
        pdi = self.pdi_read(PDI_ID.TARGET_SYSTEM_VOLTAGE_OUTPUT)
        if pdi and isinstance(pdi, PDI_FloatConfiguration):
            return pdi.value
        return None

    def get_output_voltage(self) -> Optional[float]:
        """
        Gets the latest output voltage reading from the node
        Returns:
            Output voltage in volts
        """
        return self._net_client.read_sensor_data(self._node_id, SensorType.SENSOR_OUTPUT_VOLTAGE)

    def get_input_voltage(self) -> Optional[float]:
        """
        Gets the latest input voltage reading from the node
        Returns:
            Input voltage in volts
        """
        return self._net_client.read_sensor_data(self._node_id, SensorType.SENSOR_INPUT_VOLTAGE)

    def await_sensor_value(
        self,
        sensor: SensorType.ValueType,
        target: float,
        tolerance: Optional[float] = 0.01,
        timeout: float = 5.0,
        comparison: Callable[[float, float], bool] = operator.ge,
        sampling_delay: float = 0.5,
    ) -> Optional[float]:
        """
        Waits for the sensor to report a target value, or times out.
        Args:
            sensor: The sensor to read from
            target: The target value to wait for
            tolerance: The acceptable tolerance around the target value (percent). Use None for strict comparison match.
            timeout: How long to wait for the voltage to reach the target
            comparison: The comparison function to use to compare the voltage to the target
            sampling_delay: How long to wait between sensor readings

        Returns:
            The data that matched the target, or None if the operation timed out
        """
        start_time = time.time()
        actual = None
        while (time.time() - start_time) < timeout:
            # Take another reading from the sensor
            actual = self._net_client.read_sensor_data(self._node_id, sensor)

            # Calculate the percent error and check if it is within the specified tolerance
            pct_error = abs(actual - target) / target
            if tolerance is None:
                tolerance_met = True
            else:
                tolerance_met = pct_error <= tolerance

            # Check if the sensor reading matches the targeted conditions
            if actual is not None and comparison(actual, target) and tolerance_met:
                return actual

            # Sensor reading did not match the target, wait and try again
            time.sleep(sampling_delay)

        # Timed out!
        if actual is not None:
            logger.warning(f"Sensor value {actual:.2f} did not meet target {target:.2f}")
            return actual

        return None

    def set_output_current_target(self, target: float) -> bool:
        """
        Sets the output current limit configuration of a node to a target value
        Args:
            target: The desired output current limit in amperes

        Returns:
            True if the operation succeeded, False if not
        """
        assert 0.0 <= target <= self._rated_system_output_current, f"Output current out of range: {target:.2f} A"
        return self.pdi_write(PDI_ID.TARGET_SYSTEM_CURRENT_OUTPUT, PDI_FloatConfiguration(value=target))

    def get_output_current_target(self) -> Optional[float]:
        """
        Reads the output current limit configuration on the node
        Returns:
            Output current limit in amperes
        """
        pdi = self.pdi_read(PDI_ID.TARGET_SYSTEM_CURRENT_OUTPUT)
        if pdi and isinstance(pdi, PDI_FloatConfiguration):
            return pdi.value
        return None

    def get_output_current(self) -> Optional[float]:
        """
        Reads the latest output current sensor reading from the node. This is measuring a shunt
        resistor placed in series between the power ground return path from the load and the
        switching power supply.

        Returns:
            Output current in amperes
        """
        return self._net_client.read_sensor_data(self._node_id, SensorType.SENSOR_OUTPUT_CURRENT)
