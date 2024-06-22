/******************************************************************************
 *  File Name:
 *    gpio.hpp
 *
 *  Description:
 *    Ichnaea GPIO driver interface
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_HW_GPIO_HPP
#define ICHNAEA_HW_GPIO_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>

namespace HW::GPIO
{
  /*---------------------------------------------------------------------------
  Enumerations
  ---------------------------------------------------------------------------*/

  /**
   * @brief Available GPIO pins on the Ichnaea hardware
   *
   * These are only pins dedicated to logical GPIO functions. They do not
   * include pins dedicated for other purposes like UART, SPI, etc.
   */
  enum Pin : uint32_t
  {
    PIN_LTC_DCM,
    PIN_LTC_CCM,
    PIN_LTC_RUN,
    PIN_ADC_SEL_0,
    PIN_ADC_SEL_1,
    PIN_ADC_SEL_2,
    PIN_SPI_CS_0,

    NUM_OPTIONS
  };

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initalize GPIO driver and hardware
   */
  void initialize();

  /**
   * @brief Sets the state of a GPIO pin
   *
   * @param pin Which pin to set
   * @param state  The state to set the pin to
   */
  void set( const uint32_t pin, const bool state );

  /**
   * @brief Reads the state of a GPIO pin
   *
   * @param pin Which pin to read
   * @return true   The pin is logic high
   * @return false  The pin is logic low
   */
  bool get( const uint32_t pin );

}  // namespace HW::GPIO

#endif  /* !ICHNAEA_HW_GPIO_HPP */
