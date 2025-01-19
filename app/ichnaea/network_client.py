import copy
import time
from dataclasses import dataclass
from typing import List, Optional, Dict
from threading import RLock
from loguru import logger

from ichnaea.messages import *
from mbedutils.rpc.client import RPCClient
from mbedutils.rpc.message import get_module_messages
from mbedutils.rpc.observer import MessageObserver
from mbedutils.rpc.pipe import PipeType


class NetworkClient:
    """
    Interface to all Ichnaea nodes in the distributed network, providing high-level control and monitoring.
    This class is primarily used as a gateway through the which all node communication is performed. It's
    possible to connect through any physical Ichnaea node and access the rest of the network, so this class
    more or less presents a facade to the entire system and allows querying/controlling all nodes at once.
    """

    _node_lifetime = 15.0  # How long to keep a node in the observed list before pruning

    @dataclass
    class NodeProperties:
        """Storage for node attributes observed in the system"""

        node_id: str  # Unique identifier of the node
        sw_version: str  # Firmware version of the node
        last_seen: float  # System time the node was last seen at
        heartbeat: HeartbeatPBMsg  # Last received heartbeat message

    @staticmethod
    def unique_id_to_string(unique_id: int) -> str:
        """
        Converts a node's unique identifier to a formatted string for the purpose of readability.
        The ID can be quite long and this method provides consistent behavior.
        Args:
            unique_id: Unique identifier returned from the node

        Returns:
            Formatted string representation of the unique identifier
        """
        return hex(unique_id)

    @staticmethod
    def unique_id_from_string(unique_id: str) -> int:
        """
        Converts a formatted string back into a node's unique identifier
        Args:
            unique_id: Formatted string representation of the unique identifier

        Returns:
            The unique identifier as an integer
        """
        return int(unique_id, 16)

    def __init__(self, pipe_type: PipeType, port: str, baud: int):
        """
        Args:
            pipe_type: Connection type to Ichnaea
            port: Which port to connect on
            baud: Baud rate for the connection
        """
        self._client = RPCClient()
        self._rpc_port = port
        self._rpc_baud = baud
        self._rpc_conn_type = pipe_type
        self._msg_observers = set()
        self._observed_nodes: Dict[str, NetworkClient.NodeProperties] = {}
        self._data_lock = RLock()

        # Add the message descriptors to the client
        msg_types = []
        msg_types.extend(get_module_messages("ichnaea.messages"))
        for msg_type in msg_types:
            self._client.com_pipe.add_message_descriptor(msg_type)

    @property
    def available_nodes(self) -> List[str]:
        with self._data_lock:
            return list(self._observed_nodes.keys())

    @property
    def rpc_client(self) -> RPCClient:
        return self._client

    def open(self) -> None:
        """
        Initializes the connection to the Ichnaea system
        Returns:
            None
        """
        # Open the connection
        self._client.open(pipe_type=self._rpc_conn_type, port=self._rpc_port, baud=self._rpc_baud)

        # Register an observer for heartbeat messages. This allows us to detect liveness.
        hb_obs = MessageObserver(func=self._node_heartbeat_observer, msg_type=HeartbeatPBMsg)
        hb_uuid = self._client.com_pipe.subscribe_observer(hb_obs)
        self._msg_observers.add(hb_uuid)

    def close(self) -> None:
        """
        Tear down the connection and any other acquired resources
        Returns:
            None
        """
        self._client.close()

    def discover_nodes(self, exp_count: int = -1, timeout: float = 5.0) -> List[NodeProperties]:
        """
        Broadcast to all nodes in the system and request their unique identifiers
        Args:
            exp_count: The number of nodes expected to be in the system. Set to -1 to ignore this check.
            timeout: The amount of time to wait for nodes to respond

        Returns:
            List of tuples containing the unique identifier and the firmware version of each node
        """
        self._prune_observed_nodes()

        start_time = time.time()
        responses = self._client.com_pipe.write_and_wait(
            msg=GetIdRequestPBMsg(), timeout=timeout, count=exp_count if exp_count > 0 else None
        )
        # No valid response for any requested number of nodes
        if not responses:
            return []

        # Double check that all responses are of the correct type
        assert all(
            [isinstance(r, GetIdResponsePBMsg) for r in responses]
        ), f"Invalid response type in discovery: {responses}"

        # Wait for all nodes that responded to the ID request to send a heartbeat. This
        # ensures that node is in a state where it can be interacted with.
        id_nodes = [self.unique_id_to_string(r.unique_id) for r in responses]

        while (time.time() - start_time) < timeout:
            time.sleep(0.1)
            hb_nodes = self.available_nodes
            if hb_nodes and all([n in hb_nodes for n in id_nodes]):
                break

        # Return the list of live nodes that responded to the broadcast
        with self._data_lock:
            return [self._observed_nodes[n] for n in id_nodes]

    def is_alive(self, node_id: str, liveness_window: float = 3.0) -> bool:
        """
        Checks the internal observer to see if this particular node's heartbeat
        has been received within some timeout window.
        Args:
            node_id: Which node to check
            liveness_window: Window the last heartbeat must be received to be considered alive

        Returns:
            True if alive, False otherwise
        """
        with self._data_lock:
            self._prune_observed_nodes()
            if (node := self._observed_nodes.get(node_id, None)) is not None:
                return (time.time() - node.last_seen) < liveness_window
            else:
                return False

    def get_status(self, node_id: str) -> Optional[SystemStatusResponsePBMsg]:
        """
        Queries a node for its current status
        Args:
            node_id: Which node to query

        Returns:
            The status response message from the node
        """
        msg = SystemStatusRequestPBMsg()
        msg.pb_message.node_id = self.unique_id_from_string(node_id)

        response = self._client.com_pipe.write_and_wait(msg=msg, timeout=1.0)
        if response and isinstance(response[0], SystemStatusResponsePBMsg):
            return response[0]
        else:
            logger.error(f"Failed to get status from node {node_id}")
            return None

    def get_last_heartbeat(self, node_id: str) -> Optional[HeartbeatPBMsg]:
        """
        Returns the last received heartbeat message from a node
        Args:
            node_id: Which node to query

        Returns:
            The last received heartbeat message
        """
        with self._data_lock:
            self._prune_observed_nodes()
            if (node := self._observed_nodes.get(node_id, None)) is not None:
                return node.heartbeat
            else:
                return None

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
        msg.pb_message.node_id = self.unique_id_from_string(node_id)

        sub_id = self._client.com_pipe.subscribe(msg=PingNodeResponsePBMsg, qty=1, timeout=1.0)
        self._client.com_pipe.write(msg=msg)
        response = self._client.com_pipe.get_subscription_data(sub_id)
        logger.trace(f"Ping {'succeeded' if response else 'failed'}")

        return len(response) > 0

    def send_command(self, node_id: str, command: int, timeout: float = 1.0) -> bool:
        """
        Sends a simple command to a node. These are usually used to trigger some action without a
        complex response beyond a simple status code and maybe a message.

        Args:
            node_id: The unique identifier of the node to send the command to
            command: The command to send
            timeout: How long to wait for a response. Set to <=0 to not wait for a response and immediately return.

        Returns:
            True if the command was successful, False if not
        """
        msg = ManagerRequestPBMsg()
        msg.pb_message.node_id = self.unique_id_from_string(node_id)
        msg.pb_message.command = command

        # If the timeout is 0, don't wait for a response
        if timeout <= 0:
            self._client.com_pipe.write(msg=msg)
            return True

        # Otherwise, wait for a response
        response = self._client.com_pipe.write_and_wait(msg=msg, timeout=timeout)
        if not response or not isinstance(response[0], ManagerResponsePBMsg):
            logger.error(f"Missing response from command {command} on node {node_id}")
            return False

        if response[0].pb_message.status == ERR_CMD_NO_ERROR:
            return True
        else:
            logger.error(f"Failed to execute command {command} on {node_id}: {response[0].pb_message.message}")
            return False

    def pdi_read(self, node_id: str, data_id: int) -> bytes:
        """
        Reads a program data item from a node
        Args:
            node_id: Which node to read from
            data_id: The specific data item to read

        Returns:
            The data item as raw bytes
        """
        msg = PDIReadRequestPBMsg()
        msg.pb_message.node_id = self.unique_id_from_string(node_id)
        msg.pb_message.pdi_id = data_id

        response = self._client.com_pipe.write_and_wait(msg=msg, timeout=1.0)
        if response and isinstance(response[0], PDIReadResponsePBMsg) and response[0].pb_message.success:
            return response[0].pb_message.data
        else:
            logger.error(f"Failed to read PDI {data_id} on node {node_id}")
            return b""

    def pdi_write(self, node_id: str, data_id: int, data: bytes) -> bool:
        """
        Writes a program data item to a node
        Args:
            node_id: Which node to write to
            data_id: The specific data item to write
            data: The data to write

        Returns:
            True if the operation succeeded, False if not
        """
        if len(data) > 512:
            logger.error(f"Data item too large to write to PDI {data_id} on node {node_id}")
            return False

        msg = PDIWriteRequestPBMsg()
        msg.pb_message.node_id = self.unique_id_from_string(node_id)
        msg.pb_message.pdi_id = data_id
        msg.pb_message.data = data

        response = self._client.com_pipe.write_and_wait(msg=msg, timeout=1.0)
        if response and isinstance(response[0], PDIWriteResponsePBMsg) and response[0].pb_message.success:
            return True
        else:
            logger.error(f"Failed to write PDI {data_id} on node {node_id}")
            return False

    def read_sensor_data(self, node_id: str, sensor: SensorType.ValueType) -> Optional[float]:
        """
        Reads a sensor value from a node
        Args:
            node_id: Which node to query
            sensor: Which sensor to read

        Returns:
            The sensor value
        """
        msg = SensorRequestPBMsg()
        msg.pb_message.node_id = self.unique_id_from_string(node_id)
        msg.pb_message.sensor = sensor

        response = self._client.com_pipe.write_and_wait(msg=msg, timeout=3.0)
        if response and isinstance(response[0], SensorResponsePBMsg) and response[0].status == ERR_SENSOR_NO_ERROR:
            return response[0].value

        logger.error(f"Failed to read sensor {sensor} on node {node_id}")
        return None

    def _prune_observed_nodes(self) -> None:
        """
        Prunes the list of observed nodes to remove any that have not been seen in a while
        Returns:
            None
        """
        with self._data_lock:
            now = time.time()
            old_node_ids = self._observed_nodes.keys()
            self._observed_nodes = {
                k: v for k, v in self._observed_nodes.items() if now - v.last_seen < self._node_lifetime
            }
            new_node_ids = self._observed_nodes.keys()
            pruned_nodes = set(old_node_ids) - set(new_node_ids)

            if pruned_nodes:
                logger.info(f"Node(s) {','.join(pruned_nodes)} offline")

    def _node_heartbeat_observer(self, msg: HeartbeatPBMsg) -> None:
        """
        Callback to log heartbeat messages
        Args:
            msg: Received heartbeat

        Returns:
            None
        """
        with self._data_lock:
            node_id = self.unique_id_to_string(msg.pb_message.node_id)
            if node_id not in self._observed_nodes:
                logger.info(f"Node {node_id} online")

            self._observed_nodes[node_id] = self.NodeProperties(
                node_id=node_id, sw_version="unknown", last_seen=time.time(), heartbeat=copy.copy(msg)
            )
