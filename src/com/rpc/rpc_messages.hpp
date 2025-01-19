/******************************************************************************
 *  File Name:
 *    rpc_messages.hpp
 *
 *  Description:
 *    Ichnaea RPC message definitions
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_RPC_MESSAGES_HPP
#define ICHNAEA_RPC_MESSAGES_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/rpc.hpp>
#include <src/app/proto/ichnaea_rpc.pb.h>

namespace COM::RPC
{
  using namespace mb::rpc::message;

  /*---------------------------------------------------------------------------
  Message Definitions
  ---------------------------------------------------------------------------*/

  static constexpr Descriptor PingNodeRequest{ ichnaea_Message_MSG_PING_NODE_REQ, ichnaea_MessageVersion_MSG_VER_PING_NODE_REQ,
                                               ichnaea_PingNodeRequest_fields, ichnaea_PingNodeRequest_size };

  static constexpr Descriptor PingNodeResponse{ ichnaea_Message_MSG_PING_NODE_RSP, ichnaea_MessageVersion_MSG_VER_PING_NODE_RSP,
                                                ichnaea_PingNodeResponse_fields, ichnaea_PingNodeResponse_size };

  static constexpr Descriptor GetIdRequest{ ichnaea_Message_MSG_GET_ID_REQ, ichnaea_MessageVersion_MSG_VER_GET_ID_REQ,
                                            ichnaea_GetIdRequest_fields, ichnaea_GetIdRequest_size };

  static constexpr Descriptor GetIdResponse{ ichnaea_Message_MSG_GET_ID_RSP, ichnaea_MessageVersion_MSG_VER_GET_ID_RSP,
                                             ichnaea_GetIdResponse_fields, ichnaea_GetIdResponse_size };

  static constexpr Descriptor SetpointRequest{ ichnaea_Message_MSG_SETPOINT_REQ, ichnaea_MessageVersion_MSG_VER_SETPOINT_REQ,
                                               ichnaea_SetpointRequest_fields, ichnaea_SetpointRequest_size };

  static constexpr Descriptor SetpointResponse{ ichnaea_Message_MSG_SETPOINT_RSP, ichnaea_MessageVersion_MSG_VER_SETPOINT_RSP,
                                                ichnaea_SetpointResponse_fields, ichnaea_SetpointResponse_size };

  static constexpr Descriptor SensorRequest{ ichnaea_Message_MSG_SENSOR_REQ, ichnaea_MessageVersion_MSG_VER_SENSOR_REQ,
                                             ichnaea_SensorRequest_fields, ichnaea_SensorRequest_size };

  static constexpr Descriptor SensorResponse{ ichnaea_Message_MSG_SENSOR_RSP, ichnaea_MessageVersion_MSG_VER_SENSOR_RSP,
                                              ichnaea_SensorResponse_fields, ichnaea_SensorResponse_size };

  static constexpr Descriptor ManagerRequest{ ichnaea_Message_MSG_MANAGER_REQ, ichnaea_MessageVersion_MSG_VER_MANAGER_REQ,
                                              ichnaea_ManagerRequest_fields, ichnaea_ManagerRequest_size };

  static constexpr Descriptor ManagerResponse{ ichnaea_Message_MSG_MANAGER_RSP, ichnaea_MessageVersion_MSG_VER_MANAGER_RSP,
                                               ichnaea_ManagerResponse_fields, ichnaea_ManagerResponse_size };

  static constexpr Descriptor PDIReadRequest{ ichnaea_Message_MSG_PDI_READ_REQ, ichnaea_MessageVersion_MSG_VER_PDI_READ_REQ,
                                              ichnaea_PDIReadRequest_fields, ichnaea_PDIReadRequest_size };

  static constexpr Descriptor PDIReadResponse{ ichnaea_Message_MSG_PDI_READ_RSP, ichnaea_MessageVersion_MSG_VER_PDI_READ_RSP,
                                               ichnaea_PDIReadResponse_fields, ichnaea_PDIReadResponse_size };

  static constexpr Descriptor PDIWriteRequest{ ichnaea_Message_MSG_PDI_WRITE_REQ, ichnaea_MessageVersion_MSG_VER_PDI_WRITE_REQ,
                                               ichnaea_PDIWriteRequest_fields, ichnaea_PDIWriteRequest_size };

  static constexpr Descriptor PDIWriteResponse{ ichnaea_Message_MSG_PDI_WRITE_RSP, ichnaea_MessageVersion_MSG_VER_PDI_WRITE_RSP,
                                                ichnaea_PDIWriteResponse_fields, ichnaea_PDIWriteResponse_size };

  static constexpr Descriptor SystemStatusRequest{ ichnaea_Message_MSG_SYSTEM_STATUS_REQ, ichnaea_MessageVersion_MSG_VER_SYSTEM_STATUS_REQ,
                                                   ichnaea_SystemStatusRequest_fields, ichnaea_SystemStatusRequest_size };

  static constexpr Descriptor SystemStatusResponse{ ichnaea_Message_MSG_SYSTEM_STATUS_RSP, ichnaea_MessageVersion_MSG_VER_SYSTEM_STATUS_RSP,
                                                    ichnaea_SystemStatusResponse_fields, ichnaea_SystemStatusResponse_size };
}    // namespace COM::RPC

#endif /* !ICHNAEA_RPC_MESSAGES_HPP */
