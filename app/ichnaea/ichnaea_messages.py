from ichnaea.proto.ichnaea_pb2 import *
from mbedutils.rpc.message import BasePBMsg


class GetIdRequestPBMsg(BasePBMsg[GetIdRequest]):

    def __init__(self):
        super().__init__()
        self._pb_msg = GetIdRequest()
        self.pb_message.header.msgId = MSG_GET_ID_REQ
        self.pb_message.header.version = MSG_VER_GET_ID_REQ
        self.pb_message.header.svcId = SVC_IDENTITY
        self.pb_message.header.seqId = 0


class GetIdResponsePBMsg(BasePBMsg[GetIdResponse]):
    def __init__(self):
        super().__init__()
        self._pb_msg = GetIdResponse()
        self.pb_message.header.msgId = MSG_GET_ID_RSP
        self.pb_message.header.version = MSG_VER_GET_ID_RSP
        self.pb_message.header.svcId = SVC_IDENTITY
        self.pb_message.header.seqId = 0
        self.pb_message.unique_id = 0
        self.pb_message.ver_major = 0
        self.pb_message.ver_minor = 0
        self.pb_message.ver_patch = 0

    @property
    def unique_id(self) -> int:
        return self.pb_message.unique_id

    @property
    def ver_major(self) -> int:
        return self.pb_message.ver_major

    @property
    def ver_minor(self) -> int:
        return self.pb_message.ver_minor

    @property
    def ver_patch(self) -> int:
        return self.pb_message.ver_patch


class ManagerRequestPBMsg(BasePBMsg[ManagerRequest]):
    def __init__(self):
        super().__init__()
        self._pb_msg = ManagerRequest()
        self.pb_message.header.msgId = MSG_MANAGER_REQ
        self.pb_message.header.version = MSG_VER_MANAGER_REQ
        self.pb_message.header.svcId = SVC_MANAGER
        self.pb_message.header.seqId = 0
        self.pb_message.command = 0
        self.pb_message.node_id = 0

    @property
    def command(self) -> int:
        return self.pb_message.command

    @property
    def node_id(self) -> int:
        return self.pb_message.node_id


class ManagerResponsePBMsg(BasePBMsg[ManagerResponse]):
    def __init__(self):
        super().__init__()
        self._pb_msg = ManagerResponse()
        self.pb_message.header.msgId = MSG_MANAGER_RSP
        self.pb_message.header.version = MSG_VER_MANAGER_RSP
        self.pb_message.header.svcId = SVC_MANAGER
        self.pb_message.header.seqId = 0
        self.pb_message.status = 0
        self.pb_message.message = ""

    @property
    def status(self) -> int:
        return self.pb_message.status

    @property
    def message(self) -> str:
        return self.pb_message.message


class SetpointRequestPBMsg(BasePBMsg[SetpointRequest]):
    def __init__(self):
        super().__init__()
        self._pb_msg = SetpointRequest()
        self.pb_message.header.msgId = MSG_SETPOINT_REQ
        self.pb_message.header.version = MSG_VER_SETPOINT_REQ
        self.pb_message.header.svcId = SVC_SETPOINT
        self.pb_message.header.seqId = 0
        self.pb_message.node_id = 0
        self.pb_message.field = 0
        self.pb_message.uint32_type = 0
        self.pb_message.float_type = 0.0


class SetpointResponsePBMsg(BasePBMsg[SetpointResponse]):
    def __init__(self):
        super().__init__()
        self._pb_msg = SetpointResponse()
        self.pb_message.header.msgId = MSG_SETPOINT_RSP
        self.pb_message.header.version = MSG_VER_SETPOINT_RSP
        self.pb_message.header.svcId = SVC_SETPOINT
        self.pb_message.header.seqId = 0
        self.pb_message.status = 0
        self.pb_message.message = ""

    @property
    def status(self) -> int:
        return self.pb_message.status

    @property
    def message(self) -> str:
        return self.pb_message.message


class SensorRequestPBMsg(BasePBMsg[SensorRequest]):
    def __init__(self):
        super().__init__()
        self._pb_msg = SensorRequest()
        self.pb_message.header.msgId = MSG_SENSOR_REQ
        self.pb_message.header.version = MSG_VER_SENSOR_REQ
        self.pb_message.header.svcId = SVC_SENSOR
        self.pb_message.header.seqId = 0
        self.pb_message.node_id = 0
        self.pb_message.sensor = 0


class SensorResponsePBMsg(BasePBMsg[SensorResponse]):
    def __init__(self):
        super().__init__()
        self._pb_msg = SensorResponse()
        self.pb_message.header.msgId = MSG_SENSOR_RSP
        self.pb_message.header.version = MSG_VER_SENSOR_RSP
        self.pb_message.header.svcId = SVC_SENSOR
        self.pb_message.header.seqId = 0
        self.pb_message.status = 0
        self.pb_message.value = 0.0

    @property
    def status(self) -> int:
        return self.pb_message.status

    @property
    def value(self) -> float:
        return self.pb_message.value


class PingNodeRequestPBMsg(BasePBMsg[PingNodeRequest]):
    def __init__(self):
        super().__init__()
        self._pb_msg = PingNodeRequest()
        self.pb_message.header.msgId = MSG_PING_NODE_REQ
        self.pb_message.header.version = MSG_VER_PING_NODE_REQ
        self.pb_message.header.svcId = SVC_PING_NODE
        self.pb_message.header.seqId = 0
        self.pb_message.node_id = 0


class PingNodeResponsePBMsg(BasePBMsg[PingNodeResponse]):
    def __init__(self):
        super().__init__()
        self._pb_msg = PingNodeResponse()
        self.pb_message.header.msgId = MSG_PING_NODE_RSP
        self.pb_message.header.version = MSG_VER_PING_NODE_RSP
        self.pb_message.header.svcId = SVC_PING_NODE
        self.pb_message.header.seqId = 0


class LTCRegisterSetRequestPBMsg(BasePBMsg[LTCRegisterSetRequest]):
    def __init__(self):
        super().__init__()
        self._pb_msg = LTCRegisterSetRequest()
        self.pb_message.header.msgId = MSG_LTC_REG_SET_REQ
        self.pb_message.header.version = MSG_VER_LTC_REG_SET_REQ
        self.pb_message.header.svcId = SVC_LTC_REG_SET
        self.pb_message.header.seqId = 0
        self.pb_message.node_id = 0
        self.pb_message.reg = 0
        self.pb_message.value = 0
        

class LTCRegisterSetResponsePBMsg(BasePBMsg[LTCRegisterSetResponse]):
    def __init__(self):
        super().__init__()
        self._pb_msg = LTCRegisterSetResponse()
        self.pb_message.header.msgId = MSG_LTC_REG_SET_RSP
        self.pb_message.header.version = MSG_VER_LTC_REG_SET_RSP
        self.pb_message.header.svcId = SVC_LTC_REG_SET
        self.pb_message.header.seqId = 0
        self.pb_message.status = 0


class LTCRegisterGetRequestPBMsg(BasePBMsg[LTCRegisterGetRequest]):
    def __init__(self):
        super().__init__()
        self._pb_msg = LTCRegisterGetRequest()
        self.pb_message.header.msgId = MSG_LTC_REG_GET_REQ
        self.pb_message.header.version = MSG_VER_LTC_REG_GET_REQ
        self.pb_message.header.svcId = SVC_LTC_REG_GET
        self.pb_message.header.seqId = 0
        self.pb_message.node_id = 0
        self.pb_message.reg = 0


class LTCRegisterGetResponsePBMsg(BasePBMsg[LTCRegisterGetResponse]):
    def __init__(self):
        super().__init__()
        self._pb_msg = LTCRegisterGetResponse()
        self.pb_message.header.msgId = MSG_LTC_REG_GET_RSP
        self.pb_message.header.version = MSG_VER_LTC_REG_GET_RSP
        self.pb_message.header.svcId = SVC_LTC_REG_GET
        self.pb_message.header.seqId = 0
        self.pb_message.status = 0
        self.pb_message.value = 0