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
#include "src/system/system_shutdown.hpp"
#include <src/bsp/board_map.hpp>
#include <src/sim/sim_lifetime.hpp>
#include <src/system/system_bootup.hpp>
#include <src/system/system_error.hpp>
#include <src/threads/ichnaea_threads.hpp>
#include <mbedutils/threading.hpp>

#if defined( ICHNAEA_SIMULATOR )
#include <csignal>
#include <iostream>
#endif    // ICHNAEA_SIMULATOR

/*-----------------------------------------------------------------------------
Public Functions
-----------------------------------------------------------------------------*/

#if defined( ICHNAEA_SIMULATOR )
/**
 * @brief Signal handler for graceful shutdown
 *
 * @param signal Signal number
 */
void signalHandler( int signal )
{
  std::cout << "Signal received: " << signal << std::endl;
  System::Shutdown::initiate();
  System::Shutdown::Internal::wait_for_system_teardown();
  exit( signal );
}
#endif   // ICHNAEA_SIMULATOR

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
#if defined( ICHNAEA_SIMULATOR )
  std::signal( SIGINT, signalHandler );
  std::signal( SIGTERM, signalHandler );
#endif   // ICHNAEA_SIMULATOR

  /*---------------------------------------------------------------------------
  Start the world
  ---------------------------------------------------------------------------*/
  // HW drivers must be initialized first
  System::Boot::initDrivers();

  // Sim functions (if present) build on top of the HW drivers
  SIM::initialize();

  // Finally the application threads can be started
  mb::thread::startScheduler();

  /*---------------------------------------------------------------------------
  Should never reach this point
  ---------------------------------------------------------------------------*/
  Panic::throwError( Panic::ErrorCode::ERR_SYSTEM_THREAD_EXIT );
  return -1;
}
