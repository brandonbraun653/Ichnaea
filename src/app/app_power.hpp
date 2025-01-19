/******************************************************************************
 *  File Name:
 *    app_power.hpp
 *
 *  Description:
 *    Application power management routines. This allows for very high level
 *    logical control over a generic power converter implementation.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_APP_POWER_HPP
#define ICHNAEA_APP_POWER_HPP

namespace App::Power
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initializes the power management system
   */
  void driver_init();

  /**
   * @brief Deinitializes the power management system
   */
  void driver_deinit();

  /**
   * @brief Engages the power output stage in the current configuration
   * @return True if the output was successfully engaged, false otherwise
   */
  bool engageOutput();

  /**
   * @brief Removes power from the output stage
   */
  void disengageOutput();

  /**
   * @brief Single step the core controller of the power stage.
   *
   * This should be called periodically to ensure the power stage is operating
   * correctly. It will handle all the necessary state transitions and
   * fault checking.
   */
   void periodicProcessing();

  /**
   * @brief Set the output voltage to a specific value.
   *
   * @param voltage New output voltage to set (Volts)
   */
  void setOutputVoltage( const float voltage );

  /**
   * @brief Set the output current limit.
   *
   * @param current New output current limit to set (Amps)
   */
  void setOutputCurrentLimit( const float current );

}  // namespace App::Power

#endif  /* !ICHNAEA_APP_POWER_HPP */
