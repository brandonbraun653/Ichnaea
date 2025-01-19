/******************************************************************************
 *  File Name:
 *    async_messages.hpp
 *
 *  Description:
 *    Ichnaea Async Message Definitions
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_ASYNC_MESSAGES_HPP
#define ICHNAEA_ASYNC_MESSAGES_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/rpc.hpp>
#include <src/app/proto/ichnaea_async.pb.h>

namespace COM::MSG
{
  using namespace mb::rpc::message;

  /*---------------------------------------------------------------------------
  Message Definitions
  ---------------------------------------------------------------------------*/

  static constexpr Descriptor Heartbeat{ ichnaea_AsyncMessageId_MSG_HEARTBEAT, ichnaea_AsyncMessageVersion_MSG_VER_HEARTBEAT,
                                         ichnaea_Heartbeat_fields, ichnaea_Heartbeat_size };
}  // namespace COM::MSG

#endif  /* !ICHNAEA_ASYNC_MESSAGES_HPP */
