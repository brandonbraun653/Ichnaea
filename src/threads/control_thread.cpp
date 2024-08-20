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
#include "src/com/ctrl_server.hpp"
#include "src/threads/ichnaea_threads.hpp"
#include "src/hw/ltc7871.hpp"
#include <mbedutils/logging.hpp>

namespace Threads
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void controlThread( void *arg )
  {
    sleep_ms( 100 );

    while( 1 )
    {
      sleep_ms( 25 );

      /*-----------------------------------------------------------------------
      Update the system with any new control commands, data, etc.
      -----------------------------------------------------------------------*/
      Control::getRPCServer().runServices();

      /*-----------------------------------------------------------------------
      Consume new system state to make control decisions
      -----------------------------------------------------------------------*/
      HW::LTC7871::stepController();
    }
  }
}    // namespace Threads
