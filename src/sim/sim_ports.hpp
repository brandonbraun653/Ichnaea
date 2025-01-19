/******************************************************************************
 *  File Name:
 *    sim_ports.hpp
 *
 *  Description:
 *    Port mappings for the simulation environment
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_SIM_PORT_MAPPINGS_HPP
#define ICHNAEA_SIM_PORT_MAPPINGS_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <string>

namespace SIM
{
  /*---------------------------------------------------------------------------
  ZMQ Configuration
  ---------------------------------------------------------------------------*/
  static const std::string kZMQ_EP_PATH   = "/tmp/ichnaea_sim";
  static const std::string kZMQ_EP_PREFIX = "ipc://" + kZMQ_EP_PATH + "/";

  /*---------------------------------------------------------------------------
  Hardware Interface Ports
  ---------------------------------------------------------------------------*/

  static const std::string kUART_DEBUG_PORT = "5555";
  static const std::string kUART_BMS_PORT   = "5556";

  /*---------------------------------------------------------------------------
  gRPC Service Ports
  ---------------------------------------------------------------------------*/

  static const std::string kEnvironmentSpooferPort = "50051";
}    // namespace SIM

#endif /* !ICHNAEA_SIM_PORT_MAPPINGS_HPP */
