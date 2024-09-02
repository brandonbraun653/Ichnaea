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


  /**
   * @brief Specific sensing elements in the system
   */
  enum class Element : uint8_t
  {
    /*-------------------------------------------------------------------------
    Version 1+
    -------------------------------------------------------------------------*/
    RP2040_TEMP,      /**< Internal temperature sensor on the RP2040 (Celcius)*/
    BOARD_TEMP_0,     /**< External temperature sensor 0 (Celcius)*/
    BOARD_TEMP_1,     /**< External temperature sensor 1 (Celcius)*/
    IMON_LTC_AVG,     /**< Average current output from the LTC7871 (Amps) */
    VMON_SOLAR_INPUT, /**< Solar high voltage input sense (Volts) */
    VMON_BATT_OUTPUT, /**< Buck converter voltage output sense (Volts) */

    /*-------------------------------------------------------------------------
    Version 2+
    -------------------------------------------------------------------------*/
    IMON_BATT, /**< Battery charge current (Amps)*/
    VMON_1V1,  /**< 1.1V voltage rail (Votls) */
    VMON_3V3,  /**< 3.3V voltage rail (Votls) */
    VMON_5V0,  /**< 5.0V voltage rail (Votls) */
    VMON_12V,  /**< 12V voltage rail (Volts) */

    NUM_OPTIONS
  };


  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Get the specific system sensor measurement
   *
   * @param channel Which sensor to read
   * @param lut    How to look up the data
   * @return float
   */
  float getMeasurement( const Element channel, const LookupType lut = LookupType::CACHED );

}    // namespace Sensor

#endif /* !ICHNAEA_SYSTEM_SENSOR_HPP */
