/******************************************************************************
 *  File Name:
 *    main.cpp
 *
 *  Description:
 *    Main entry point for Ichnaea
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "pico/multicore.h"
#include "src/hw/bootup.hpp"
#include "src/system/system_error.hpp"
#include "src/threads/ichnaea_threads.hpp"

/*-----------------------------------------------------------------------------
Public Functions
-----------------------------------------------------------------------------*/

/**
 * @brief Core entry point for Ichnaea
 *
 * This is responsible for initializing the hardware and starting the main
 * monitor and control system threads.
 *
 * @return int
 */
int main()
{
  /*---------------------------------------------------------------------------
  Start the world
  ---------------------------------------------------------------------------*/
  HW::initDrivers();
  HW::runPostInit();

  /*---------------------------------------------------------------------------
  Spin up the monitor and control threads. There is no true OS here, so a
  "thread" is just a function that consumes a whole core.
  ---------------------------------------------------------------------------*/
  multicore_launch_core1( Threads::monitorThread );
  Threads::controlThread();

  /*---------------------------------------------------------------------------
  Should never reach this point
  ---------------------------------------------------------------------------*/
  Panic::throwSystemError( Panic::ErrorCode::SYSTEM_THREAD_EXIT );
  return -1;
}
