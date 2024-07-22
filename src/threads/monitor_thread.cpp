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
#include "src/app/app_decl.hpp"
#include "src/bsp/board_map.hpp"
#include "src/threads/ichnaea_threads.hpp"
#include "src/system/system_sensor.hpp"
#include <mbedutils/logging.hpp>

namespace Threads
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void monitorThread( void *arg )
  {
    sleep_ms( 1500 );
    while( 1 )
    {
      LOG_INFO( "Current: %.2fA, VHigh: %.2fV, VLow: %.2f", Sensor::getAverageCurrent(), Sensor::getHighSideVoltage(),
                Sensor::getLowSideVoltage() );
      sleep_ms( 1000 );
    }
  }
}    // namespace Threads
