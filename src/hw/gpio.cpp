/******************************************************************************
 *  File Name:
 *    gpio.cpp
 *
 *  Description:
 *    Ichnaea GPIO driver implementation
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "etl/array.h"
#include "src/hw/gpio.hpp"
#include "src/bsp/board_map.hpp"

namespace HW::GPIO
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    /*-------------------------------------------------------------------------
    Set all RP2040 pins to input w/pulldown so we start from a known state
    -------------------------------------------------------------------------*/
    for( size_t rp_pin = 0; rp_pin < NUM_BANK0_GPIOS; ++rp_pin )
    {
      gpio_init( rp_pin );
      gpio_set_dir( rp_pin, GPIO_IN );
      gpio_set_pulls( rp_pin, false, true );
    }

    /*-------------------------------------------------------------------------
    Set Ichnaea specific pins to OUTPUT LOW. Most pins are active high so this
    should leave them in a safe state.
    -------------------------------------------------------------------------*/
    auto cfg = BSP::getIOConfig();
    for( size_t port = 0; port < BSP::GPIO_MAX_PORTS; ++port )
    {
      gpio_init( cfg.gpio[ port ].pin );
      gpio_set_dir( cfg.gpio[ port ].pin, GPIO_OUT );
      gpio_put( cfg.gpio[ port ].pin, false );
    }

    /*-------------------------------------------------------------------------
    Set any active low pins to high to disable them.
    -------------------------------------------------------------------------*/
    gpio_put( cfg.gpio[ BSP::GPIO_SPI_CS0 ].pin, true );
  }


  void set( const uint32_t pin, const bool state )
  {
    if( pin < BSP::GPIO_MAX_PORTS )
    {
      gpio_put( BSP::getIOConfig().gpio[ pin ].pin, state );
    }
  }


  bool get( const uint32_t pin )
  {
    if( pin < BSP::GPIO_MAX_PORTS )
    {
      return gpio_get( BSP::getIOConfig().gpio[ pin ].pin );
    }

    return false;
  }
}  // namespace HW::GPIO
