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
    RP2040_TEMP,  /**< Internal temperature sensor on the RP2040 */
    LTC_IMON,     /**< Average current output from the LTC7871 */
    TEMP_SENSE_0, /**< External temperature sensor 0 */
    TEMP_SENSE_1, /**< External temperature sensor 1 */
    HV_DC_SENSE,  /**< Solar high voltage input sense */
    LV_DC_SENSE,  /**< Buck converter voltage output sense */

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
