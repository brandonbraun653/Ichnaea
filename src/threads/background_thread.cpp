/******************************************************************************
 *  File Name:
 *    background_thread.cpp
 *
 *  Description:
 *    Background process for Idle things and low priority tasks
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "src/bsp/board_map.hpp"
#include "src/threads/ichnaea_threads.hpp"
#include "src/hw/led.hpp"
#include "src/hw/bootup.hpp"
#include <mbedutils/logging.hpp>

namespace Threads
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/
  void backgroundThread( void *arg )
  {

    HW::runPostInit();
    // TODO: Send message to all other threads to start

    HW::LED::setBrightness( HW::LED::Channel::STATUS_0, 0.5f );
    while( 1 )
    {
      HW::LED::toggle( HW::LED::Channel::STATUS_0 );
      sleep_ms( 500 );
    }
  }
}  // namespace Threads
