/******************************************************************************
 *  File Name:
 *    system_sensor.hpp
 *
 *  Description:
 *    Ichnaea system data inspection and manipulation routines
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_SYSTEM_SENSOR_HPP
#define ICHNAEA_SYSTEM_SENSOR_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>

namespace Sensor
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Gets the average current reported by the LTC7871 (Amps)
   *
   * @return float
   */
  float getAverageCurrent();

  /**
   * @brief Gets the solar panel high side input (Volts)
   *
   * @return float
   */
  float getHighSideVoltage();

  /**
   * @brief Gets the regulated low side output voltage (Volts)
   *
   * @return float
   */
  float getLowSideVoltage();

  /**
   * @brief Gets the temperature of the RP2040 (Celsius)
   *
   * @return float
   */
  float getRP2040Temp();

  /**
   * @brief Gets the board temperature from sensor 0 (Celsius)
   * @warning This will not give a valid reading unless the LTC7871 is in normal operation.
   *
   * @return float
   */
  float getBoardTemp0();

  /**
   * @brief Gets the board temperature from sensor 1 (Celsius)
   *
   * @return float
   */
  float getBoardTemp1();

}  // namespace Data

#endif  /* !ICHNAEA_SYSTEM_SENSOR_HPP */
