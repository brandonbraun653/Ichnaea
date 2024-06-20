/******************************************************************************
 *  File Name:
 *    led.cpp
 *
 *  Description:
 *    LED driver implementation
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "etl/array.h"
#include "src/bsp/board_map.hpp"
#include "src/hw/led.hpp"

namespace HW::LED
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  static constexpr bool LED_ON  = 0;
  static constexpr bool LED_OFF = 1;

  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static etl::array<uint, Channel::NUM_OPTIONS> s_led_map;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    /*-------------------------------------------------------------------------
    Initialize the LED mapping
    -------------------------------------------------------------------------*/
    s_led_map[ Channel::STATUS_0 ] = BSP::getIOConfig().gpio.ledStatus0;
    s_led_map[ Channel::STATUS_1 ] = BSP::getIOConfig().gpio.ledStatus1;
    s_led_map[ Channel::STATUS_2 ] = BSP::getIOConfig().gpio.ledStatus2;
    s_led_map[ Channel::STATUS_3 ] = BSP::getIOConfig().gpio.ledStatus3;

    /*-------------------------------------------------------------------------
    Initialize the GPIO pins
    -------------------------------------------------------------------------*/
    for( auto &led : s_led_map )
    {
      gpio_init( led );
      gpio_pull_up( led );
      gpio_set_dir( led, GPIO_OUT );
      gpio_put( led, 1 );
    }
  }


  void postSequence()
  {
    /*-------------------------------------------------------------------------
    Quickly pulse each led to indicate the system is booting
    -------------------------------------------------------------------------*/
    for( auto &led : s_led_map )
    {
      gpio_put( led, LED_ON );
      sleep_ms( 100 );
      gpio_put( led, LED_OFF );
    }
  }


  void set( const Channel channel )
  {
    if( channel < s_led_map.size() )
    {
      gpio_put( s_led_map[ channel ], LED_ON );
    }
  }


  void clear( const Channel channel )
  {
    if( channel < s_led_map.size() )
    {
      gpio_put( s_led_map[ channel ], LED_OFF );
    }
  }


  void toggle( const Channel channel )
  {
    if( channel < s_led_map.size() )
    {
      gpio_put( s_led_map[ channel ], !gpio_get( s_led_map[ channel ] ) );
    }
  }

}    // namespace HW::LED
