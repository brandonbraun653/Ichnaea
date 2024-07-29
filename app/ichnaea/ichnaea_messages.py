from ichnaea.proto.ichnaea_pb2 import *
from mbedutils.rpc.message import BasePBMsg


class GetIdRequestPBMsg(BasePBMsg[GetIdRequest]):

    def __init__(self):
        super().__init__()
        self._pb_msg = GetIdRequest()
        self._pb_msg.header.msgId = MSG_GET_ID_REQ
        self._pb_msg.header.version = MSG_VER_GET_ID_REQ
        self._pb_msg.header.svcId = SVC_IDENTITY
        self._pb_msg.header.seqId = 0


class GetIdResponsePBMsg(BasePBMsg[GetIdResponse]):
    def __init__(self):
        super().__init__()
        self._pb_msg = GetIdResponse()
        self._pb_msg.header.msgId = MSG_GET_ID_RSP
        self._pb_msg.header.version = MSG_VER_GET_ID_RSP
        self._pb_msg.header.svcId = SVC_IDENTITY
        self._pb_msg.header.seqId = 0
        self._pb_msg.unique_id = 0
        self._pb_msg.ver_major = 0
        self._pb_msg.ver_minor = 0
        self._pb_msg.ver_patch = 0

    @property
    def unique_id(self) -> int:
        return self._pb_msg.unique_id

    @property
    def ver_major(self) -> int:
        return self._pb_msg.ver_major

    @property
    def ver_minor(self) -> int:
        return self._pb_msg.ver_minor

    @property
    def ver_patch(self) -> int:
        return self._pb_msg.ver_patch


class ManagerRequestPBMsg(BasePBMsg[ManagerRequest]):
    def __init__(self):
        super().__init__()
        self._pb_msg = ManagerRequest()
        self._pb_msg.header.msgId = MSG_MANAGER_REQ
        self._pb_msg.header.version = MSG_VER_MANAGER_REQ
        self._pb_msg.header.svcId = SVC_MANAGER
        self._pb_msg.header.seqId = 0
        self._pb_msg.command = 0
        self._pb_msg.node_id = 0

    @property
    def command(self) -> int:
        return self._pb_msg.command

    @property
    def node_id(self) -> int:
        return self._pb_msg.node_id


class ManagerResponsePBMsg(BasePBMsg[ManagerResponse]):
    def __init__(self):
        super().__init__()
        self._pb_msg = ManagerResponse()
        self._pb_msg.header.msgId = MSG_MANAGER_RSP
        self._pb_msg.header.version = MSG_VER_MANAGER_RSP
        self._pb_msg.header.svcId = SVC_MANAGER
        self._pb_msg.header.seqId = 0
        self._pb_msg.status = 0
        self._pb_msg.message = ""

    @property
    def status(self) -> int:
        return self._pb_msg.status

    @property
    def message(self) -> str:
        return self._pb_msg.message


class SetpointRequestPBMsg(BasePBMsg[SetpointRequest]):
    def __init__(self):
        super().__init__()
        self._pb_msg = SetpointRequest()
        self._pb_msg.header.msgId = MSG_SETPOINT_REQ
        self._pb_msg.header.version = MSG_VER_SETPOINT_REQ
        self._pb_msg.header.svcId = SVC_SETPOINT
        self._pb_msg.header.seqId = 0
        self._pb_msg.node_id = 0
        self._pb_msg.field = 0
        self._pb_msg.uint32_type = 0
        self._pb_msg.float_type = 0.0


class SetpointResponsePBMsg(BasePBMsg[SetpointResponse]):
    def __init__(self):
        super().__init__()
        self._pb_msg = SetpointResponse()
        self._pb_msg.header.msgId = MSG_SETPOINT_RSP
        self._pb_msg.header.version = MSG_VER_SETPOINT_RSP
        self._pb_msg.header.svcId = SVC_SETPOINT
        self._pb_msg.header.seqId = 0
        self._pb_msg.status = 0
        self._pb_msg.message = ""

    @property
    def status(self) -> int:
        return self._pb_msg.status

    @property
    def message(self) -> str:
        return self._pb_msg.message


class SensorRequestPBMsg(BasePBMsg[SensorRequest]):
    def __init__(self):
        super().__init__()
        self._pb_msg = SensorRequest()
        self._pb_msg.header.msgId = MSG_SENSOR_REQ
        self._pb_msg.header.version = MSG_VER_SENSOR_REQ
        self._pb_msg.header.svcId = SVC_SENSOR
        self._pb_msg.header.seqId = 0
        self._pb_msg.node_id = 0
        self._pb_msg.sensor = 0


class SensorResponsePBMsg(BasePBMsg[SensorResponse]):
    def __init__(self):
        super().__init__()
        self._pb_msg = SensorResponse()
        self._pb_msg.header.msgId = MSG_SENSOR_RSP
        self._pb_msg.header.version = MSG_VER_SENSOR_RSP
        self._pb_msg.header.svcId = SVC_SENSOR
        self._pb_msg.header.seqId = 0
        self._pb_msg.status = 0
        self._pb_msg.value = 0.0

    @property
    def status(self) -> int:
        return self._pb_msg.status

    @property
    def value(self) -> float:
        return self._pb_msg.value


class PingNodeRequestPBMsg(BasePBMsg[PingNodeRequest]):
    def __init__(self):
        super().__init__()
        self._pb_msg = PingNodeRequest()
        self._pb_msg.header.msgId = MSG_PING_NODE_REQ
        self._pb_msg.header.version = MSG_VER_PING_NODE_REQ
        self._pb_msg.header.svcId = SVC_PING_NODE
        self._pb_msg.header.seqId = 0
        self._pb_msg.node_id = 0


class PingNodeResponsePBMsg(BasePBMsg[PingNodeResponse]):
    def __init__(self):
        super().__init__()
        self._pb_msg = PingNodeResponse()
        self._pb_msg.header.msgId = MSG_PING_NODE_RSP
        self._pb_msg.header.version = MSG_VER_PING_NODE_RSP
        self._pb_msg.header.svcId = SVC_PING_NODE
        self._pb_msg.header.seqId = 0
