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
#include <mbedutils/logging.hpp>

namespace Threads
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void controlThread( void *arg )
  {
    /*-------------------------------------------------------------------------
    Bind builtin services and messages to the server
    -------------------------------------------------------------------------*/
    sleep_ms( 100 );

    while( 1 )
    {
      //Control::getRPCServer().runServices();
      sleep_ms( 25 );
    }
  }
}    // namespace Threads
