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
#include <mbedutils/logging.hpp>
#include <mbedutils/threading.hpp>
#include <src/app/app_power.hpp>
#include <src/com/ctrl_server.hpp>
#include <src/hw/ltc7871.hpp>
#include <src/threads/ichnaea_threads.hpp>

namespace Threads
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void controlThread( void *arg )
  {
    ( void )arg;

    while( 1 )
    {
      /*-----------------------------------------------------------------------
      Perform work periodically
      -----------------------------------------------------------------------*/
      mb::thread::this_thread::sleep_for( 25 );

      /*-----------------------------------------------------------------------
      Check for a kill request
      -----------------------------------------------------------------------*/
      if( mb::thread::this_thread::task()->killPending() )
      {
        break;
      }

      /*-----------------------------------------------------------------------
      Update the system with any new control commands, data, etc.
      -----------------------------------------------------------------------*/
      Control::getRPCServer().runServices();

      /*-----------------------------------------------------------------------
      Consume new system state to make control decisions
      -----------------------------------------------------------------------*/
      App::Power::periodicProcessing();
    }

    /*-------------------------------------------------------------------------
    Shutdown sequence
    -------------------------------------------------------------------------*/
    LOG_INFO( "Control thread shutting down" );
  }
}    // namespace Threads
