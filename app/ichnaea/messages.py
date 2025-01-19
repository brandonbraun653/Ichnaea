from ichnaea.proto.ichnaea_rpc_pb2 import *
from ichnaea.proto.ichnaea_async_pb2 import *
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

class PDIReadRequestPBMsg(BasePBMsg[PDIReadRequest]):
    def __init__(self):
        super().__init__()
        self._pb_msg = PDIReadRequest()
        self.pb_message.header.msgId = MSG_PDI_READ_REQ
        self.pb_message.header.version = MSG_VER_PDI_READ_REQ
        self.pb_message.header.svcId = SVC_PDI_READ
        self.pb_message.header.seqId = 0
        self.pb_message.node_id = 0
        self.pb_message.pdi_id = 0


class PDIReadResponsePBMsg(BasePBMsg[PDIReadResponse]):
    def __init__(self):
        super().__init__()
        self._pb_msg = PDIReadResponse()
        self.pb_message.header.msgId = MSG_PDI_READ_RSP
        self.pb_message.header.version = MSG_VER_PDI_READ_RSP
        self.pb_message.header.svcId = SVC_PDI_READ
        self.pb_message.header.seqId = 0
        self.pb_message.success = False
        self.pb_message.data = b''


class PDIWriteRequestPBMsg(BasePBMsg[PDIWriteRequest]):
    def __init__(self):
        super().__init__()
        self._pb_msg = PDIWriteRequest()
        self.pb_message.header.msgId = MSG_PDI_WRITE_REQ
        self.pb_message.header.version = MSG_VER_PDI_WRITE_REQ
        self.pb_message.header.svcId = SVC_PDI_WRITE
        self.pb_message.header.seqId = 0
        self.pb_message.node_id = 0
        self.pb_message.pdi_id = 0
        self.pb_message.data = b''


class PDIWriteResponsePBMsg(BasePBMsg[PDIWriteResponse]):
    def __init__(self):
        super().__init__()
        self._pb_msg = PDIWriteResponse()
        self.pb_message.header.msgId = MSG_PDI_WRITE_RSP
        self.pb_message.header.version = MSG_VER_PDI_WRITE_RSP
        self.pb_message.header.svcId = SVC_PDI_WRITE
        self.pb_message.header.seqId = 0
        self.pb_message.success = False


class HeartbeatPBMsg(BasePBMsg[Heartbeat]):
    def __init__(self):
        super().__init__()
        self._pb_msg = Heartbeat()
        self.pb_message.header.msgId = MSG_HEARTBEAT
        self.pb_message.header.version = MSG_VER_HEARTBEAT
        self.pb_message.header.svcId = 0  # Not actually used
        self.pb_message.header.seqId = 0
        self.pb_message.node_id = 0
        self.pb_message.boot_count = 0
        self.pb_message.timestamp = 0


class SystemStatusRequestPBMsg(BasePBMsg[SystemStatusRequest]):
    def __init__(self):
        super().__init__()
        self._pb_msg = SystemStatusRequest()
        self.pb_message.header.msgId = MSG_SYSTEM_STATUS_REQ
        self.pb_message.header.version = MSG_VER_SYSTEM_STATUS_REQ
        self.pb_message.header.svcId = SVC_SYSTEM_STATUS
        self.pb_message.header.seqId = 0
        self.pb_message.node_id = 0


class SystemStatusResponsePBMsg(BasePBMsg[SystemStatusResponse]):
    def __init__(self):
        super().__init__()
        self._pb_msg = SystemStatusResponse()
        self.pb_message.header.msgId = MSG_SYSTEM_STATUS_RSP
        self.pb_message.header.version = MSG_VER_SYSTEM_STATUS_RSP
        self.pb_message.header.svcId = SVC_SYSTEM_STATUS
        self.pb_message.header.seqId = 0
        self.pb_message.timestamp = 0
        self.pb_message.output_state = 0
