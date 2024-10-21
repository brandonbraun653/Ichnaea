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
      for( size_t i = 0; i < static_cast<size_t>( Sensor::Element::NUM_OPTIONS ); i++ )
      {
        Sensor::getMeasurement( static_cast<Sensor::Element>( i ), Sensor::LookupType::REFRESH );
      }

      // TODO BMB: I'm going to need to detect and announce an event where the
      // input voltage drops out and the system is running on battery power. I
      // technically need to do a full reset of the LTC7871 to ensure we power
      // back on appropriately.
    }
  }
}    // namespace Threads
