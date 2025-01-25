/******************************************************************************
 *  File Name:
 *    app_power.cpp
 *
 *  Description:
 *    Application power management routines
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/app_power.hpp>
#include <src/app/pdi/config_max_system_voltage_input.hpp>
#include <src/app/pdi/config_max_system_voltage_input.hpp>
#include <src/app/pdi/config_min_system_voltage_input.hpp>
#include <src/app/pdi/max_system_voltage_input_rated_limit.hpp>
#include <src/app/pdi/min_system_voltage_input_rated_limit.hpp>
#include <src/app/pdi/pgood_monitor_timeout_ms.hpp>
#include <src/app/pdi/phase_current_output_rated_limit.hpp>
#include <src/app/pdi/system_current_output_rated_limit.hpp>
#include <src/app/pdi/system_voltage_output_rated_limit.hpp>
#include <src/app/pdi/target_phase_current_output.hpp>
#include <src/app/pdi/target_system_current_output.hpp>
#include <src/app/pdi/target_system_voltage_output.hpp>
#include <src/app/pdi/mon_output_current.hpp>
#include <src/app/pdi/mon_output_voltage.hpp>
#include <src/app/pdi/mon_input_voltage.hpp>
#include <src/app/pdi/mon_fan_speed.hpp>
#include <src/app/pdi/mon_temperature.hpp>
#include <src/app/proto/ichnaea_pdi.pb.h>
#include <src/hw/led.hpp>
#include <src/hw/ltc7871.hpp>
#include <src/system/system_db.hpp>
#include <src/system/system_sensor.hpp>

namespace App::Power
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  static constexpr float INVALID_SETPOINT_REQUEST = -1.0f;

  /*---------------------------------------------------------------------------
  Private Data
  ---------------------------------------------------------------------------*/

  static bool  s_power_output_enabled;
  static float s_voltage_request;
  static float s_current_request;

  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/

  static bool is_voltage_target_valid( const float request )
  {
    const float vin_max  = App::PDI::getConfigMaxSystemVoltageInput();
    const float vin_min  = App::PDI::getConfigMinSystemVoltageInput();
    const float vout_lim = App::PDI::getSystemVoltageOutputRatedLimit();
    const float vin_act  = System::Sensor::getMeasurement( System::Sensor::Element::VMON_SOLAR_INPUT );

    bool valid = true;

    valid &= request >= 0.0f;        // Does not fall below zero
    valid &= request <= vin_max;     // Does not exceed configured max input voltage
    valid &= request >= vin_min;     // Does fall below configured min input voltage
    valid &= request <= vout_lim;    // Does not exceed electrical system output voltage limit
    valid &= request <= vin_act;     // Does not exceed actual input voltage (Buck converter requirement)

    return valid;
  }


  static bool is_current_target_valid( const float request )
  {
    const float iout_lim       = App::PDI::getSystemCurrentOutputRatedLimit();
    const float iout_phase_lim = App::PDI::getPhaseCurrentOutputRatedLimit();

    bool valid = true;

    valid &= request >= 0.0f;                         // Does not fall below zero
    valid &= request <= iout_lim;                     // Does not exceed electrical system output current limit
    valid &= ( request / 6.0f ) <= iout_phase_lim;    // Does not exceed phase current limit

    return valid;
  }

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void driver_init()
  {
    /*-------------------------------------------------------------------------
    Initialize module data
    -------------------------------------------------------------------------*/
    s_power_output_enabled = false;
    s_voltage_request      = INVALID_SETPOINT_REQUEST;
    s_current_request      = INVALID_SETPOINT_REQUEST;

    /*-------------------------------------------------------------------------
    Register the PDI keys for the power management system
    -------------------------------------------------------------------------*/
    App::PDI::pdi_register_key__config_max_system_voltage_input();
    App::PDI::pdi_register_key__config_min_system_voltage_input();
    App::PDI::pdi_register_key__system_current_output_rated_limit();
    App::PDI::pdi_register_key__system_voltage_output_rated_limit();
    App::PDI::pdi_register_key__max_system_voltage_input_rated_limit();
    App::PDI::pdi_register_key__min_system_voltage_input_rated_limit();
    App::PDI::pdi_register_key__pgood_monitor_timeout_ms();
    App::PDI::pdi_register_key__phase_current_output_rated_limit();
    App::PDI::pdi_register_key__target_phase_current_output();
    App::PDI::pdi_register_key__target_system_current_output();
    App::PDI::pdi_register_key__target_system_voltage_output();
  }


  void driver_deinit()
  {
  }


  bool engageOutput()
  {
    /*-------------------------------------------------------------------------
    Check if the output is already enabled
    -------------------------------------------------------------------------*/
    if( s_power_output_enabled )
    {
      return true;
    }

    /*-------------------------------------------------------------------------
    Gather all the necessary data for the power on sequence
    -------------------------------------------------------------------------*/
    const float vin_max  = App::PDI::getConfigMaxSystemVoltageInput();
    const float vin_min  = App::PDI::getConfigMinSystemVoltageInput();
    const float vout_lim = App::PDI::getSystemVoltageOutputRatedLimit();
    const float vout_tgt = App::PDI::getTargetSystemVoltageOutput();
    const float iout_tgt = App::PDI::getTargetSystemCurrentOutput();
    const float vin_act  = System::Sensor::getMeasurement( System::Sensor::Element::VMON_SOLAR_INPUT );
    const float vout_act = System::Sensor::getMeasurement( System::Sensor::Element::VMON_LOAD );
    const float iout_act = System::Sensor::getMeasurement( System::Sensor::Element::IMON_LOAD );

    /*-------------------------------------------------------------------------
    Check range of the input and configuration values
    -------------------------------------------------------------------------*/
    bool can_engage = true;

    // Basic validity of setpoints
    can_engage &= mbed_assert_continue_msg( ( vout_tgt > 0.0f ) && is_voltage_target_valid( vout_tgt ),
                                            "Invalid voltage target: %.2fV", vout_tgt );
    can_engage &= mbed_assert_continue_msg( ( iout_tgt > 0.0f ) && is_current_target_valid( iout_tgt ),
                                            "Invalid current target: %.2fA", iout_tgt );

    // Operational range checks on current system state
    can_engage &= mbed_assert_continue_msg( vin_act <= vin_max, "OOB Vin %.2fV > %.2fV Cfg", vin_act, vin_max );
    can_engage &= mbed_assert_continue_msg( vin_act >= vin_min, "OOB Vin %.2fV < %.2fV Cfg", vin_act, vin_min );
    can_engage &= mbed_assert_continue_msg( vout_act <= vout_lim, "OOB Vout %.2fV > %.2fV Limit", vout_act, vout_lim );
    can_engage &= mbed_assert_continue_msg( iout_act < 100e-3f, "Iout %.2fA > 100mA", iout_act );

    if( !can_engage )
    {
      return false;
    }

    /*-------------------------------------------------------------------------
    Engage the output stage
    -------------------------------------------------------------------------*/
    s_power_output_enabled = HW::LTC7871::enablePowerConverter( vout_tgt, iout_tgt );
    if( s_power_output_enabled )
    {
      App::Monitor::reset();
      App::Monitor::enable();
    }

    return s_power_output_enabled;
  }


  void disengageOutput()
  {
    /*-------------------------------------------------------------------------
    Immediately disable the power output. Need to shut that off ASAP.
    -------------------------------------------------------------------------*/
    HW::LTC7871::disablePowerConverter();

    /*-------------------------------------------------------------------------
    Perform cleanup operations for lower priority systems.
    -------------------------------------------------------------------------*/
    App::Monitor::disable();
    App::Monitor::reset();

    /*-------------------------------------------------------------------------
    Reset controller setpoints to ensure a new configuration is specified
    before re-enabling the power output.
    -------------------------------------------------------------------------*/
    App::PDI::setTargetSystemVoltageOutput( 0.0f );
    App::PDI::setTargetSystemCurrentOutput( 0.0f );

    s_power_output_enabled = false;
    s_voltage_request      = INVALID_SETPOINT_REQUEST;
    s_current_request      = INVALID_SETPOINT_REQUEST;
  }


  bool setOutputVoltage( const float voltage )
  {
    if( !is_voltage_target_valid( voltage ) )
    {
      return false;
    }

    s_voltage_request = voltage;
    return true;
  }


  bool setOutputCurrentLimit( const float current )
  {
    if( !is_current_target_valid( current ) )
    {
      return false;
    }

    s_current_request = current;
    return true;
  }


  void periodicProcessing()
  {
    HW::LTC7871::runStateUpdater();

    switch( HW::LTC7871::getMode() )
    {
      /*-----------------------------------------------------------------------
      The power stage is enabled and running normally. Update the setpoints
      and monitor for faults.
      -----------------------------------------------------------------------*/
      case HW::LTC7871::DriverMode::ENABLED:
        if( s_voltage_request >= 0.0f )
        {
          App::PDI::setTargetSystemVoltageOutput( s_voltage_request );
          HW::LTC7871::setVoutRef( s_voltage_request );
          s_voltage_request = INVALID_SETPOINT_REQUEST;
        }

        if( s_current_request >= 0.0f )
        {
          App::PDI::setTargetSystemCurrentOutput( s_current_request );
          HW::LTC7871::setIoutRef( s_current_request );
          s_current_request = INVALID_SETPOINT_REQUEST;
        }

        HW::LTC7871::runFaultMonitoring();
        break;

      /*-----------------------------------------------------------------------
      The power stage is faulted. Notify the user and wait for a reset.
      -----------------------------------------------------------------------*/
      case HW::LTC7871::DriverMode::FAULTED:
        HW::LED::enable( HW::LED::Channel::FAULT );
        break;

      case HW::LTC7871::DriverMode::DISABLED:
      default:
        break;
    }
  }

}    // namespace App::Power
