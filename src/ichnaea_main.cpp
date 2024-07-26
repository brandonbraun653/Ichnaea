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
#include <mbedutils/thread.hpp>

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
  mb::thread::startScheduler();

  /*---------------------------------------------------------------------------
  Should never reach this point
  ---------------------------------------------------------------------------*/
  Panic::throwError( Panic::ErrorCode::ERR_SYSTEM_THREAD_EXIT );
  return -1;
}
