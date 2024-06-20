/******************************************************************************
 *  File Name:
 *    led.hpp
 *
 *  Description:
 *    Interface to the system LEDs
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_HW_LED_HPP
#define ICHNAEA_HW_LED_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>

namespace HW::LED
{
  /*---------------------------------------------------------------------------
  Enumerations
  ---------------------------------------------------------------------------*/
  enum Channel : uint8_t
  {
    STATUS_0,
    STATUS_1,
    STATUS_2,
    STATUS_3,

    NUM_OPTIONS
  };

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initializes the LED hardware
   */
  void initialize();

  /**
   * @brief Runs the POST sequence on the LEDs
   */
  void postSequence();

  /**
   * @brief Turns on the LED
   *
   * @param channel   Which LED to turn on
   */
  void set( const Channel channel );

  /**
   * @brief Turns off the LED
   *
   * @param channel   Which LED to turn off
   */
  void clear( const Channel channel );

  /**
   * @brief Toggles the logical state of the led
   *
   * @param channel   Which LED to toggle
   */
  void toggle( const Channel channel );

}    // namespace HW::LED

#endif /* !ICHNAEA_HW_LED_HPP */
