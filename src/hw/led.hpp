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
  enum class Channel : uint8_t
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
   *  Initializes the LED hardware
   *
   *  @return void
   */
  void initialize();

}    // namespace HW::LED

#endif /* !ICHNAEA_HW_LED_HPP */
