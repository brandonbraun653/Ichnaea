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
