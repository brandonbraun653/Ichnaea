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
#include "src/hw/fan.hpp"
#include "src/threads/ichnaea_threads.hpp"
#include "src/system/system_sensor.hpp"
#include <mbedutils/logging.hpp>

namespace Threads
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void controlThread( void )
  {
    while( 1 )
    {
      LOG_INFO( "Current: %.2fA, VHigh: %.2fV, VLow: %.2f", Sensor::getAverageCurrent(), Sensor::getHighSideVoltage(),
                Sensor::getLowSideVoltage() );
      LOG_INFO( "Fan Speed: %.2f RPM", HW::FAN::getFanSpeed() );
      sleep_ms( 1000 );
    }
  }
}    // namespace Threads
