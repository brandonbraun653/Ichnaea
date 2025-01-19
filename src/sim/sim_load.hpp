/******************************************************************************
 *  File Name:
 *    sim_load.hpp
 *
 *  Description:
 *    Load modeling for the simulation environment
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_SIM_LOAD_HPP
#define ICHNAEA_SIM_LOAD_HPP

namespace SIM::Load
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Prepare the load modeler for operation
   */
  void initialize();

  /**
   * @brief Reset all loads to a high impedance state
   */
  void reset();

  /**
   * @brief Get the immediate current flowing through the load
   *
   * @return Current in Amperes
   */
  float currentNow();

  /**
   * @brief Apply a purely resistive load to the system
   *
   * @param resistance Desired resistance in ohms
   */
  void setResistiveLoad( const float resistance );

}  // namespace SIM::Load

#endif  /* !ICHNAEA_SIM_LOAD_HPP */
