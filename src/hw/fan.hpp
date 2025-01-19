/******************************************************************************
 *  File Name:
 *    fan.hpp
 *
 *  Description:
 *    Ichnaea fan control driver implementation
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_FAN_CONTROL_HPP
#define ICHNAEA_FAN_CONTROL_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/


namespace HW::FAN
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initialize the fan control hardware
   */
  void initialize();

  /**
   * @brief POST sequence for the fan control hardware
   */
  void postSequence();

  /**
   * @brief Set the fan speed as a percentage of maximum
   *
   * @param speed  Value between 0.0 and 1.0
   */
  void setSpeedRPM( const float speed );

  /**
   * @brief Gets the fan speed in RPM
   *
   * @return float Fan speed in RPM
   */
  float getFanSpeed();
}  // namespace HW::FAN

#endif  /* !ICHNAEA_FAN_CONTROL_HPP */
