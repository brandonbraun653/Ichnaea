/******************************************************************************
 *  File Name:
 *    sim_lifetime.hpp
 *
 *  Description:
 *    Startup and shutdown routines for the simulation environment
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_SIM_LIFETIME_HPP
#define ICHNAEA_SIM_LIFETIME_HPP

namespace SIM
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initialize the simulation environment
   */
  void initialize();

  /**
   * @brief Shutdown the simulation environment
   */
  void shutdown();

}  // namespace SIM

#endif  /* !ICHNAEA_SIM_LIFETIME_HPP */
