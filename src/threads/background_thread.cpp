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
#include <mbedutils/logging.hpp>
#include <src/app/app_stats.hpp>
#include <src/bsp/board_map.hpp>
#include <src/hw/led.hpp>
#include <src/system/system_bootup.hpp>
#include <src/threads/ichnaea_threads.hpp>

namespace Threads
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/
  void backgroundThread( void *arg )
  {
    /*-------------------------------------------------------------------------
    Finish the remainder of the bootup sequence
    -------------------------------------------------------------------------*/
    System::Boot::initTech();
    System::Boot::runPostInit();

    /*-------------------------------------------------------------------------
    Start powering on application level drivers
    -------------------------------------------------------------------------*/
    App::Stats::initialize();

    /*-------------------------------------------------------------------------
    Signal the next thread in the sequence to start
    -------------------------------------------------------------------------*/
    // TODO BMB: Probably should start the monitor thread next.

    HW::LED::setBrightness( HW::LED::Channel::STATUS_0, 0.5f );
    while( 1 )
    {
      HW::LED::toggle( HW::LED::Channel::STATUS_0 );
      sleep_ms( 500 );
    }
  }
}  // namespace Threads
