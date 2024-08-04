from typing import List, Tuple, Optional

from loguru import logger

import ichnaea.ltc7871 as ltc
from ichnaea.ichnaea_messages import *
from mbedutils.rpc.client import RPCClient
from mbedutils.rpc.message import get_module_messages


class IchnaeaClient:

    def __init__(self, port: str, baud: int):
        self._client = RPCClient()
        self._rpc_port = port
        self._rpc_baud = baud
        self._system_nodes = set()

        # Add the message descriptors to the client
        msg_types = get_module_messages("ichnaea.ichnaea_messages")
        for msg_type in msg_types:
            self._client.com_pipe.add_message_descriptor(msg_type)

    @property
    def available_nodes(self) -> List[str]:
        return list(self._system_nodes)

    def open(self, discovery_timeout: float = 1.0) -> None:
        """
        Initializes the connection to the Ichnaea system and probes for all available nodes.

        Args:
            discovery_timeout: The amount of time to wait for nodes to respond to the discovery request.

        Returns:
            None
        """
        self._client.open(port=self._rpc_port, baud=self._rpc_baud)
        self.identify_nodes(timeout=discovery_timeout)
        logger.info(f"Connected to Ichnaea system with {len(self._system_nodes)} nodes")

    def close(self) -> None:
        self._client.close()

    def identify_nodes(self, timeout: float = 1.0) -> List[Tuple[str, str]]:
        """
        Broadcast to all nodes in the system and request their unique identifiers
        Returns:
            List of tuples containing the unique identifier and the firmware version of each node
        """
        # Set up an observer for the response messages
        sub_id = self._client.com_pipe.subscribe(msg=GetIdResponsePBMsg, qty=25, timeout=timeout)

        # Send the request message and wait for responses
        get_id_msg = GetIdRequestPBMsg()
        self._client.com_pipe.write(msg=get_id_msg)
        responses = self._client.com_pipe.get_subscription_data(sub_id)

        # Process the responses
        found_nodes = [(hex(r.unique_id), f"{r.ver_major}.{r.ver_minor}.{r.ver_patch}") for r in responses]
        [self._system_nodes.add(node_id) for node_id, version in found_nodes]
        return found_nodes

    def ping_node(self, node_id: str) -> bool:
        """
        Pings a node to verify that it is still alive
        Args:
            node_id: The unique identifier of the node to ping, from the identify_nodes() method.

        Returns:
            True if the node is alive, False if not
        """
        logger.trace(f"Pinging node {node_id}")
        msg = PingNodeRequestPBMsg()
        msg.pb_message.node_id = int(node_id, 16)

        sub_id = self._client.com_pipe.subscribe(msg=PingNodeResponsePBMsg, qty=1, timeout=1.0)
        self._client.com_pipe.write(msg=msg)
        response = self._client.com_pipe.get_subscription_data(sub_id)
        logger.trace(f"Ping {'succeeded' if response else 'failed'}")

        return len(response) > 0

    def engage_output(self, node_id: str) -> bool:
        """
        Instructs a node to power on its output stage. This requires the node to perform a safety check
        before engaging and may take some time to complete. Uses pre-programmed settings for the output
        stage.

        Args:
            node_id:

        Returns:
            True if the operation succeeded, False if not
        """
        msg = ManagerRequestPBMsg()
        msg.pb_message.node_id = int(node_id, 16)
        msg.pb_message.command = CMD_ENGAGE_OUTPUT

        response = self._client.com_pipe.write_and_wait(msg=msg, timeout=0.5)
        if not isinstance(response, ManagerResponsePBMsg):
            logger.error(f"Missing response from output engage on node {node_id}")
            return False

        if response.pb_message.status == ERR_CMD_NO_ERROR:
            return True
        else:
            logger.error(f"Failed to engage output on {node_id}: {response.pb_message.message}")
            return False

    def disengage_output(self, node_id: str) -> bool:
        """
        Instructs a node to power off the output stage, preventing power from reaching the load.
        Args:
            node_id: Which node to disengage

        Returns:
            True if the operation succeeded, False if not
        """
        msg = ManagerRequestPBMsg()
        msg.pb_message.node_id = int(node_id, 16)
        msg.pb_message.command = CMD_DISENGAGE_OUTPUT

        response = self._client.com_pipe.write_and_wait(msg=msg, timeout=0.5)
        if not isinstance(response, ManagerResponsePBMsg):
            logger.error(f"Missing response from output disengage on node {node_id}")
            return False

        if response.pb_message.status == ERR_CMD_NO_ERROR:
            return True
        else:
            logger.error(f"Failed to disengage output on {node_id}: {response.pb_message.message}")
            return False

    def write_ltc_register(self, node_id: str, address: int, value: int) -> bool:
        """
        Writes a value to an LTC7871 register on a node
        Args:
            node_id: Which node to write
            address: Which register to write
            value: The value to write

        Returns:
            True if the operation succeeded, False if not
        """
        assert 1 <= address <= 0xB
        assert 0 <= value <= 0xFF

        msg = LTCRegisterSetRequestPBMsg()
        msg.pb_message.node_id = int(node_id, 16)
        msg.pb_message.reg = address
        msg.pb_message.value = value

        response = self._client.com_pipe.write_and_wait(msg=msg, timeout=0.5)

        if not isinstance(response, LTCRegisterSetResponsePBMsg):
            logger.error(f"Missing response from LTC register write on node {node_id}")
            return False

        if not response.pb_message.status == ERR_LTC_REG_NO_ERROR:
            logger.error(f"Failed to write LTC register {address} on node {node_id}: {response.pb_message.status}")
            return False

        return True

    def read_ltc_register(self, node_id: str, address: int) -> Optional[int]:
        """
        Reads the value of an LTC7871 register on a node
        Args:
            node_id: Which node to read
            address: Which register to read

        Returns:
            The value of the register
        """
        assert 1 <= address <= 0xB

        msg = LTCRegisterGetRequestPBMsg()
        msg.pb_message.node_id = int(node_id, 16)
        msg.pb_message.reg = address

        response = self._client.com_pipe.write_and_wait(msg=msg, timeout=0.5)
        if isinstance(response, LTCRegisterGetResponsePBMsg) and response.pb_message.status == ERR_LTC_REG_NO_ERROR:
            return response.pb_message.value
        else:
            logger.error(f"Failed to read LTC register {address} on node {node_id}: {response.pb_message.status}")
            return None

    def get_input_voltage(self, node_id: str) -> Optional[float]:
        """
        Reads the input voltage of a node
        Args:
            node_id: Which node to query

        Returns:
            The input voltage in volts
        """
        return self._read_sensor_data(node_id, SENSOR_INPUT_VOLTAGE)

    def get_output_voltage(self, node_id: str) -> Optional[float]:
        """
        Reads the output voltage of a node
        Args:
            node_id: Which node to query

        Returns:
            Output voltage in volts
        """
        return self._read_sensor_data(node_id, SENSOR_OUTPUT_VOLTAGE)

    def set_output_voltage(self, node_id: str, voltage: float) -> bool:
        """
        Sets the output voltage of a node
        Args:
            node_id: Which node to set
            voltage: Desired output voltage in volts

        Returns:
            True if the operation succeeded, False if not
        """
        idac_value, _ = ltc.compute_optimal_vlow_idac(voltage)
        return self.write_ltc_register(node_id, ltc.REG_MFR_IDAC_VLOW, idac_value)

    def _read_sensor_data(self, node_id: str, sensor: int) -> Optional[float]:
        """
        Reads a sensor value from a node
        Args:
            node_id: Which node to query
            sensor: Which sensor to read

        Returns:
            The sensor value
        """
        msg = SensorRequestPBMsg()
        msg.pb_message.node_id = int(node_id, 16)
        msg.pb_message.sensor = sensor

        response = self._client.com_pipe.write_and_wait(msg=msg, timeout=0.5)
        if isinstance(response, SensorResponsePBMsg) and response.status == ERR_SENSOR_NO_ERROR:
            return response.value

        logger.error(f"Failed to read sensor {sensor} on node {node_id}")
        return None
