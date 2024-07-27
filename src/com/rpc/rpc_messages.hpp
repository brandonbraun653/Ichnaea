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
  /*---------------------------------------------------------------------------
  Message Definitions
  ---------------------------------------------------------------------------*/

  static constexpr mb::rpc::message::Descriptor GetIdRequest{ ichnaea_Message_MSG_GET_ID_REQ,
                                                              ichnaea_MessageVersion_MSG_VER_GET_ID_REQ,
                                                              ichnaea_GetIdRequest_fields, ichnaea_GetIdRequest_size };

  static constexpr mb::rpc::message::Descriptor GetIdResponse{ ichnaea_Message_MSG_GET_ID_RSP,
                                                               ichnaea_MessageVersion_MSG_VER_GET_ID_RSP,
                                                               ichnaea_GetIdResponse_fields, ichnaea_GetIdResponse_size };
}  // namespace COM::RPC

#endif  /* !ICHNAEA_RPC_MESSAGES_HPP */
