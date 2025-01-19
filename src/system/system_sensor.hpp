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

namespace System::Sensor
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
    VMON_LOAD,        /**< Buck converter voltage output sense (Volts) */

    /*-------------------------------------------------------------------------
    Version 2+
    -------------------------------------------------------------------------*/
    IMON_LOAD, /**< Load output current (Amps) */
    VMON_1V1,  /**< 1.1V voltage rail (Volts) */
    VMON_3V3,  /**< 3.3V voltage rail (Volts) */
    VMON_5V0,  /**< 5.0V voltage rail (Volts) */
    VMON_12V,  /**< 12V voltage rail (Volts) */
    FAN_SPEED, /**< Fan speed (RPM) */

    NUM_OPTIONS
  };


  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Prepare the sensor subsystem for operation
   */
  void initialize();

  /**
   * @brief Get the specific system sensor measurement
   *
   * @param channel Which sensor to read
   * @param lut    How to look up the data
   * @return float
   */
  float getMeasurement( const Element channel, const LookupType lut = LookupType::CACHED );

  namespace Calibration
  {
    /**
     * @brief Calibrate the IMON_LOAD sensor to account for offset error
     *
     * Assumes the system is in a state where no load is applied to the unit
     * and the IMON_LOAD sensor is reading the offset error.
     */
    void calibrateImonNoLoadOffset();

  }    // namespace Calibration

}    // namespace System::Sensor

#endif /* !ICHNAEA_SYSTEM_SENSOR_HPP */
