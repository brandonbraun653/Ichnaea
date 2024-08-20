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
  Enumerations
  ---------------------------------------------------------------------------*/

  /**
   * @brief Specify how data in the sensor module should be looked up
   */
  enum class LookupType : uint8_t
  {
    CACHED, /**< Use the cached value, typically for speed. */
    REFRESH /**< Access hardware and perform a new measurement. */
  };

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Gets the average current reported by the LTC7871 (Amps)
   *
   * @param lut What type of lookup to perform
   * @return float
   */
  float getAverageCurrent( const LookupType lut = LookupType::CACHED );

  /**
   * @brief Gets the solar panel high side input (Volts)
   *
   * @param lut What type of lookup to perform
   * @return float
   */
  float getHighSideVoltage( const LookupType lut = LookupType::CACHED );

  /**
   * @brief Gets the regulated low side output voltage (Volts)
   *
   * @param lut What type of lookup to perform
   * @return float
   */
  float getLowSideVoltage( const LookupType lut = LookupType::CACHED );

  /**
   * @brief Gets the temperature of the RP2040 (Celsius)
   *
   * @param lut What type of lookup to perform
   * @return float
   */
  float getRP2040Temp( const LookupType lut = LookupType::CACHED );

  /**
   * @brief Gets the board temperature from sensor 0 (Celsius)
   * @warning This will not give a valid reading unless the LTC7871 is in normal operation.
   *
   * @param lut What type of lookup to perform
   * @return float
   */
  float getBoardTemp0( const LookupType lut = LookupType::CACHED );

  /**
   * @brief Gets the board temperature from sensor 1 (Celsius)
   *
   * @param lut What type of lookup to perform
   * @return float
   */
  float getBoardTemp1( const LookupType lut = LookupType::CACHED );

}  // namespace Data

#endif  /* !ICHNAEA_SYSTEM_SENSOR_HPP */
