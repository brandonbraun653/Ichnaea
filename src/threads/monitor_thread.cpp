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
#include "src/hw/led.hpp"
#include <mbedutils/logging.hpp>

namespace Threads
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void monitorThread( void *arg )
  {
    while( 1 )
    {
      sleep_ms( 25 );

      /*-----------------------------------------------------------------------
      Update all the sensors
      -----------------------------------------------------------------------*/
      Sensor::getAverageCurrent( Sensor::LookupType::REFRESH );
      Sensor::getHighSideVoltage( Sensor::LookupType::REFRESH );
      Sensor::getLowSideVoltage( Sensor::LookupType::REFRESH );
      Sensor::getRP2040Temp( Sensor::LookupType::REFRESH );
      Sensor::getBoardTemp0( Sensor::LookupType::REFRESH );
      Sensor::getBoardTemp1( Sensor::LookupType::REFRESH );
    }
  }
}    // namespace Threads
