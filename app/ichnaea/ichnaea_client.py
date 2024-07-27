from typing import List, Tuple

from ichnaea.ichnaea_messages import GetIdResponsePBMsg, GetIdRequestPBMsg
from mbedutils.rpc.client import RPCClient


class IchnaeaClient:

    def __init__(self, port: str, baud: int):
        self._client = RPCClient()
        self._rpc_port = port
        self._rpc_baud = baud

    def connect(self) -> bool:
        # TODO BMB: Build in the idea of a token based connection here. We want to make sure that
        #    we're the only ones talking to the remote system. Have the Ichnaea unit generate a
        #    random token that we can use to authenticate further requests with.
        self._client.open(port=self._rpc_port, baud=self._rpc_baud)
        return True

    def disconnect(self) -> None:
        self._client.close()

    def identify_nodes(self) -> List[Tuple[int, str]]:
        """
        Broadcast to all nodes in the system and request their unique identifiers
        Returns:
            List of tuples containing the unique identifier and the firmware version of each node
        """
        # Set up an observer for the response messages
        sub_id = self._client.com_pipe.subscribe(msg=GetIdResponsePBMsg, qty=25, timeout=5.0)

        # Send the request message and wait for responses
        get_id_msg = GetIdRequestPBMsg()
        self._client.com_pipe.write(msg=get_id_msg)
        responses = self._client.com_pipe.get_subscription_data(sub_id)

        # Process the responses
        return [(r.id, f"{r.major}.{r.minor}.{r.patch}") for r in responses]
