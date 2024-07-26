/******************************************************************************
 *  File Name:
 *    ctrl_server.hpp
 *
 *  Description:
 *    Ichnaea control server interface. This allows for a remote system to
 *    control the system behavior of Ichnaea.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_CONTROL_SERVER_HPP
#define ICHNAEA_CONTROL_SERVER_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/rpc.hpp>

namespace Control
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initializes the various control server components
   */
  void initialize();

  mb::rpc::server::Server &getRPCServer();

}  // namespace Control

#endif  /* !ICHNAEA_CONTROL_SERVER_HPP */
