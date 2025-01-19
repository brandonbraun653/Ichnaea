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
  enum Channel : unsigned int
  {
    STATUS_0,    // Yellow
    STATUS_1,    // Yellow
    STATUS_2,    // Green
    STATUS_3,    // Green

    NUM_OPTIONS,

    // Aliases
    FAULT     = STATUS_0,
    HEARTBEAT = STATUS_3,
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
  void enable( const unsigned int channel );

  /**
   * @brief Turns off the LED
   *
   * @param channel   Which LED to turn off
   */
  void disable( const unsigned int channel );

  /**
   * @brief Toggles the logical state of the led
   *
   * @param channel   Which LED to toggle
   */
  void toggle( const unsigned int channel );

  /**
   * @brief Set the brightness of the LED when it's enabled
   *
   * @param channel   Which LED to adjust
   * @param brightness  Brightness level from 0.0 to 1.0
   */
  void setBrightness( const unsigned int channel, const float brightness );

}    // namespace HW::LED

#endif /* !ICHNAEA_HW_LED_HPP */
