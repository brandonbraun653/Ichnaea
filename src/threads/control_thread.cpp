/******************************************************************************
 *  File Name:
 *    control_thread.cpp
 *
 *  Description:
 *    Control thread to implement MPPT and other system control behaviors
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "src/bsp/board_map.hpp"
#include "src/threads/ichnaea_threads.hpp"

namespace Threads
{
  void controlThread( void )
  {
    /*---------------------------------------------------------------------------
    Initialize the GPIO pin
    ---------------------------------------------------------------------------*/
    const uint LED_PIN = 20;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while( 1 )
    {
      gpio_put( LED_PIN, 1 );
      sleep_ms( 500 );
      gpio_put( LED_PIN, 0 );
      sleep_ms( 500 );
    }
  }
}  // namespace Threads
