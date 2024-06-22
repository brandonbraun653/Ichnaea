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
    LTC_IMON,
    TEMP_SENSE_0,
    TEMP_SENSE_1,
    HV_DC_SENSE,
    LV_DC_SENSE,

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
  void postInit();

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
   * This will return the last known voltage on the specified channel. If the
   * channel has not been read yet, the result will be -1.0f.
   *
   * @param channel Which channel to read
   * @return float  The voltage on the channel
   */
  float getCachedVoltage( const size_t channel );

}  // namespace HW::ADC

#endif  /* !ICHNAEA_HW_ADC_HPP */
