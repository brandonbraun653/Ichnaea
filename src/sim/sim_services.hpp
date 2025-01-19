/******************************************************************************
 *  File Name:
 *    sim_services.hpp
 *
 *  Description:
 *    Interface to the simulation control server gRPC services. Mostly this is
 *    meant to be a very small wrapper around setup/teardown of the services.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_SIM_SERVICES_HPP
#define ICHNAEA_SIM_SERVICES_HPP

namespace SIM
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Spawns the various gRPC services for the simulator
   */
  void spawnServices();

  /**
   * @brief Destroys the various gRPC services for the simulator
   */
  void destroyServices();
}  // namespace SIM

#endif  /* !ICHNAEA_SIM_SERVICES_HPP */
