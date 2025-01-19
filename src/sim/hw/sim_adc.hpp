/******************************************************************************
 *  File Name:
 *    sim_adc.hpp
 *
 *  Description:
 *    Simulator ADC driver interface
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_HW_SIM_ADC_HPP
#define ICHNAEA_HW_SIM_ADC_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstddef>
#include <functional>
#include <src/hw/adc.hpp>

namespace HW::ADC
{
  /*---------------------------------------------------------------------------
  Aliases
  ---------------------------------------------------------------------------*/

  /**
   * @brief Update callback for the ADC simulation
   */
  using UpdateCallback = std::function<float( void )>;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Inject a new sample into the ADC simulation
   *
   * @param channel   Which ADC channel to inject the sample into
   * @param voltage   The voltage to inject
   * @param timestamp Time the sample was taken (mb::time::millis())
   */
  void inject_sample( const Channel channel, const float voltage, const size_t timestamp );

  /**
   * @brief Set the callback function to update the ADC simulation
   *
   * @param channel   Which ADC channel to set the callback for
   * @param callback  Function to call when the ADC channel is updated
   */
  void set_update_callback( const Channel channel, const UpdateCallback &callback );

}  // namespace HW::ADC

#endif  /* !ICHNAEA_HW_SIM_ADC_HPP */
