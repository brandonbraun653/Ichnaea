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
    size_t last_print = mb::time::millis();

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

      if( ( mb::time::millis() - last_print ) > 1000 )
      {
        last_print = mb::time::millis();
        LOG_INFO( "Current: %.2fA, High Side: %.2fV, Low Side: %.2fV, RP2040 Temp: %.2fC, Board Temp 0: %.2fC, Board Temp 1: %.2fC",
                  Sensor::getAverageCurrent(),
                  Sensor::getHighSideVoltage(),
                  Sensor::getLowSideVoltage(),
                  Sensor::getRP2040Temp(),
                  Sensor::getBoardTemp0(),
                  Sensor::getBoardTemp1() );
      }

      // TODO BMB: I'm going to need to detect and announce an event where the
      // input voltage drops out and the system is running on battery power. I
      // technically need to do a full reset of the LTC7871 to ensure we power
      // back on appropriately.
    }
  }
}    // namespace Threads
