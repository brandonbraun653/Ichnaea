/******************************************************************************
 *  File Name:
 *    monitor_thread.cpp
 *
 *  Description:
 *    Monitor thread to ensure that the system is operating correctly
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
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void monitorThread( void )
  {

    while( 1 )
    {
      // gpio_put( LED_PIN, 1 );
      // sleep_ms( 500 );
      // gpio_put( LED_PIN, 0 );
      sleep_ms( 500 );
    }
  }
}    // namespace Threads
