/******************************************************************************
 *  File Name:
 *    board_map.hpp
 *
 *  Description:
 *    Selects the appropriate board IO map for the current hardware.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_BOARD_MAP_HPP
#define ICHNAEA_BOARD_MAP_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "hardware/gpio.h"
#include "pico/platform.h"
#include "pico/stdlib.h"
#include <cstddef>
#include <cstdint>

#if defined( PICO_RP2040 ) && ( PICO_RP2040 == 1 )
#include "hardware/adc.h"
#include "hardware/resets.h"
#include "pico/bootrom.h"

#define ICHNAEA_EMBEDDED

#else
#define ICHNAEA_SIMULATOR
#endif /* PICO_RP2040 */

namespace BSP
{
  namespace Internal
  {
    /**
     * @brief LED pins to use in the event the board version cannot be determined.
     *
     * These must be defined here because the board version is not known at compile
     * time. Ideally these pins do not change in future hardware revisions.
     */
    static constexpr uint32_t UNMAPPED_LED_PINS[] = { 18, 19, 20, 21 };
    static constexpr uint32_t UNMAPPED_LED_NUM    = sizeof( UNMAPPED_LED_PINS ) / sizeof( UNMAPPED_LED_PINS[ 0 ] );
  }    // namespace Internal

  /*---------------------------------------------------------------------------
  Public Types
  ---------------------------------------------------------------------------*/

  /**
   * @brief Abstracts the IO map for a given hardware version
   */
  struct IOConfig
  {
    size_t majorVersion;
    size_t minorVersion;
  };

  /*-----------------------------------------------------------------------------
  Public Functions
  -----------------------------------------------------------------------------*/

  /**
   * @brief Inspects hardware to determine the appropriate IO map.
   *
   * Must be called before any other driver initialization.
   */
  void powerUp();

  /**
   * @brief Returns the IO map for the current hardware
   * @return const IOConfig&
   */
  const IOConfig &getIOConfig();
}    // namespace BSP

#endif /* !ICHNAEA_BOARD_MAP_HPP */
