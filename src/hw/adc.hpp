/******************************************************************************
 *  File Name:
 *    adc.hpp
 *
 *  Description:
 *    Ichanea ADC driver interface
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_HW_ADC_HPP
#define ICHNAEA_HW_ADC_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstddef>

namespace HW::ADC
{
  /*---------------------------------------------------------------------------
  Enumerations
  ---------------------------------------------------------------------------*/

  enum Channel : size_t
  {
    /*-------------------------------------------------------------------------
    Version 1+
    -------------------------------------------------------------------------*/
    RP2040_TEMP,  /**< Internal temperature sensor on the RP2040 */
    TEMP_SENSE_0, /**< External temperature sensor 0 */
    TEMP_SENSE_1, /**< External temperature sensor 1 */
    LTC_IMON,     /**< Average current output from the LTC7871 */
    HV_DC_SENSE,  /**< Solar high voltage input sense */
    LV_DC_SENSE,  /**< Buck converter voltage output sense */

    /*-------------------------------------------------------------------------
    Version 2+
    -------------------------------------------------------------------------*/
    BOARD_REV, /**< Board revision voltage */
    IMON_LOAD, /**< Output load current */
    VMON_1V1,  /**< 1.1V voltage rail */
    VMON_3V3,  /**< 3.3V voltage rail */
    VMON_5V0,  /**< 5.0V voltage rail */
    VMON_12V,  /**< 12V voltage rail */

    NUM_OPTIONS
  };

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initalize the ADC driver and hardware
   */
  void initialize();

  /**
   * @brief POST sequence for the ADC hardware
   */
  void postSequence();

  /**
   * @brief Read the voltage on a specific channel.
   *
   * This will immediately configure the HW to read the specified channel. It
   * may take a few microseconds for the conversion to complete. Each time
   * this function is called, the result will be cached for faster access.
   *
   * @param channel Which channel to read
   * @return float The voltage on the channel
   */
  float getVoltage( const size_t channel );

  /**
   * @brief Get the voltage on a specific channel without triggering a new conversion.
   *
   * This will return the last known voltage, which may be stale. Errors in this
   * call will return -1.0f.
   *
   * @param channel Which channel to read
   * @return float  The voltage on the channel
   */
  float getCachedVoltage( const size_t channel );

}  // namespace HW::ADC

#endif  /* !ICHNAEA_HW_ADC_HPP */
