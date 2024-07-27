

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
    def id(self) -> int:
        return self._pb_msg.id

    @property
    def major(self) -> int:
        return self._pb_msg.major

    @property
    def minor(self) -> int:
        return self._pb_msg.minor

    @property
    def patch(self) -> int:
        return self._pb_msg.patch
