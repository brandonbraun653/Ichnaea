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
#include <src/threads/ichnaea_threads.hpp>
#include <src/system/system_sensor.hpp>
#include <src/app/app_monitor.hpp>
#include <mbedutils/logging.hpp>
#include <mbedutils/threading.hpp>

namespace Threads
{
  /*---------------------------------------------------------------------------
  Static Functions
  ---------------------------------------------------------------------------*/

  void logMeasurements()
  {
    static uint32_t lastCallTime = 0;
    uint32_t        currentTime  = mb::time::millis();

    if( currentTime - lastCallTime >= 1000 )
    {
      lastCallTime = currentTime;

      auto inputVoltage  = System::Sensor::getMeasurement( System::Sensor::Element::VMON_SOLAR_INPUT );
      auto outputVoltage = System::Sensor::getMeasurement( System::Sensor::Element::VMON_LOAD );
      auto outputCurrent = System::Sensor::getMeasurement( System::Sensor::Element::IMON_LOAD );
      LOG_INFO( "Input Voltage: %.2f V, Output Voltage: %.2f V, Output Current: %.2f A", inputVoltage, outputVoltage,
                outputCurrent );
    }
  }

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void monitorThread( void *arg )
  {
    ( void )arg;

    /*-------------------------------------------------------------------------
    Start up the control thread
    -------------------------------------------------------------------------*/
    startThread( SystemTask::TSK_CONTROL_ID );

    while( !mb::thread::this_thread::task()->killPending() )
    {
      /*-------------------------------------------------------------------------
      Refresh the sensor data
      -------------------------------------------------------------------------*/
      System::Sensor::getMeasurement( System::Sensor::Element::IMON_LOAD, System::Sensor::LookupType::REFRESH );
      System::Sensor::getMeasurement( System::Sensor::Element::VMON_LOAD, System::Sensor::LookupType::REFRESH );
      System::Sensor::getMeasurement( System::Sensor::Element::VMON_SOLAR_INPUT, System::Sensor::LookupType::REFRESH );
      System::Sensor::getMeasurement( System::Sensor::Element::VMON_1V1, System::Sensor::LookupType::REFRESH );
      System::Sensor::getMeasurement( System::Sensor::Element::VMON_3V3, System::Sensor::LookupType::REFRESH );
      System::Sensor::getMeasurement( System::Sensor::Element::VMON_5V0, System::Sensor::LookupType::REFRESH );
      System::Sensor::getMeasurement( System::Sensor::Element::VMON_12V, System::Sensor::LookupType::REFRESH );
      System::Sensor::getMeasurement( System::Sensor::Element::BOARD_TEMP_0, System::Sensor::LookupType::REFRESH );
      System::Sensor::getMeasurement( System::Sensor::Element::BOARD_TEMP_1, System::Sensor::LookupType::REFRESH );
      System::Sensor::getMeasurement( System::Sensor::Element::FAN_SPEED, System::Sensor::LookupType::REFRESH );

      /*-----------------------------------------------------------------------
      Update all monitors
      -----------------------------------------------------------------------*/
      /* High Priority */
      App::Monitor::monitorOutputCurrent();
      App::Monitor::monitorOutputVoltage();
      App::Monitor::monitorInputVoltage();

      /* Lower priority */
      App::Monitor::monitor1V1Voltage();
      App::Monitor::monitor3V3Voltage();
      App::Monitor::monitor5V0Voltage();
      App::Monitor::monitor12V0Voltage();
      App::Monitor::monitorTemperature();
      App::Monitor::monitorFanSpeed();

      logMeasurements();

      /*-----------------------------------------------------------------------
      Yield to other threads
      -----------------------------------------------------------------------*/
      mb::thread::this_thread::sleep_for( 10 );
    }

    /*-------------------------------------------------------------------------
    Shutdown sequence
    -------------------------------------------------------------------------*/
    LOG_INFO( "Monitor thread shutting down" );
  }
}    // namespace Threads
