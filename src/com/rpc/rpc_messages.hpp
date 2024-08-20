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
#include "src/com/proto/ichnaea.pb.h"

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

  static constexpr Descriptor LTCRegisterGetRequest{ ichnaea_Message_MSG_LTC_REG_GET_REQ,
                                                     ichnaea_MessageVersion_MSG_VER_LTC_REG_GET_REQ,
                                                     ichnaea_LTCRegisterGetRequest_fields, ichnaea_LTCRegisterGetRequest_size };

  static constexpr Descriptor LTCRegisterGetResponse{ ichnaea_Message_MSG_LTC_REG_GET_RSP,
                                                      ichnaea_MessageVersion_MSG_VER_LTC_REG_GET_RSP,
                                                      ichnaea_LTCRegisterGetResponse_fields,
                                                      ichnaea_LTCRegisterGetResponse_size };

  static constexpr Descriptor LTCRegisterSetRequest{ ichnaea_Message_MSG_LTC_REG_SET_REQ,
                                                     ichnaea_MessageVersion_MSG_VER_LTC_REG_SET_REQ,
                                                     ichnaea_LTCRegisterSetRequest_fields, ichnaea_LTCRegisterSetRequest_size };

  static constexpr Descriptor LTCRegisterSetResponse{ ichnaea_Message_MSG_LTC_REG_SET_RSP,
                                                      ichnaea_MessageVersion_MSG_VER_LTC_REG_SET_RSP,
                                                      ichnaea_LTCRegisterSetResponse_fields,
                                                      ichnaea_LTCRegisterSetResponse_size };
}    // namespace COM::RPC

#endif /* !ICHNAEA_RPC_MESSAGES_HPP */
