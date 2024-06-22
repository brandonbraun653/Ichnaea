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
  Static Data
  ---------------------------------------------------------------------------*/

  static etl::array<size_t, Pin::NUM_OPTIONS> s_pin_map;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    /*-------------------------------------------------------------------------
    Initialize the GPIO mapping
    -------------------------------------------------------------------------*/
    memset( &s_pin_map, 0, sizeof( s_pin_map ) );

    s_pin_map[ Pin::PIN_LTC_DCM ]   = BSP::getIOConfig().gpio.ltcDcm;
    s_pin_map[ Pin::PIN_LTC_CCM ]   = BSP::getIOConfig().gpio.ltcCcm;
    s_pin_map[ Pin::PIN_LTC_RUN ]   = BSP::getIOConfig().gpio.ltcRun;
    s_pin_map[ Pin::PIN_ADC_SEL_0 ] = BSP::getIOConfig().gpio.adcSel0;
    s_pin_map[ Pin::PIN_ADC_SEL_1 ] = BSP::getIOConfig().gpio.adcSel1;
    s_pin_map[ Pin::PIN_ADC_SEL_2 ] = BSP::getIOConfig().gpio.adcSel2;
    s_pin_map[ Pin::PIN_SPI_CS_0 ]  = BSP::getIOConfig().gpio.spiCs0;

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
    for( size_t pin = 0; pin < Pin::NUM_OPTIONS; ++pin )
    {
      gpio_init( s_pin_map[ pin ] );
      gpio_set_dir( s_pin_map[ pin ], GPIO_OUT );
      gpio_put( s_pin_map[ pin ], false );
    }

    /*-------------------------------------------------------------------------
    Set any active low pins to high to disable them.
    -------------------------------------------------------------------------*/
    gpio_put( s_pin_map[ Pin::PIN_SPI_CS_0 ], true );
  }


  void set( const uint32_t pin, const bool state )
  {
    if( pin < Pin::NUM_OPTIONS )
    {
      gpio_put( s_pin_map[ pin ], state );
    }
  }


  bool get( const uint32_t pin )
  {
    if( pin < Pin::NUM_OPTIONS )
    {
      return gpio_get( s_pin_map[ pin ] );
    }

    return false;
  }
}  // namespace HW::GPIO
