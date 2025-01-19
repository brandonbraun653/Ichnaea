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
  Private Data
  ---------------------------------------------------------------------------*/

  static bool  s_power_output_enabled;
  static float s_voltage_request;
  static float s_current_request;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void driver_init()
  {
    /*-------------------------------------------------------------------------
    Initialize module data
    -------------------------------------------------------------------------*/
    s_power_output_enabled = false;
    s_voltage_request      = 0.0f;
    s_current_request      = 0.0f;

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
    const float vin_max        = App::PDI::getConfigMaxSystemVoltageInput();
    const float vin_min        = App::PDI::getConfigMinSystemVoltageInput();
    const float vout_lim       = App::PDI::getSystemVoltageOutputRatedLimit();
    const float iout_lim       = App::PDI::getSystemCurrentOutputRatedLimit();
    const float iout_phase_lim = App::PDI::getPhaseCurrentOutputRatedLimit();
    const float vout_tgt       = App::PDI::getTargetSystemVoltageOutput();
    const float iout_tgt       = App::PDI::getTargetSystemCurrentOutput();
    const float vin_act        = System::Sensor::getMeasurement( System::Sensor::Element::VMON_SOLAR_INPUT );
    const float vout_act       = System::Sensor::getMeasurement( System::Sensor::Element::VMON_LOAD );
    const float iout_act       = System::Sensor::getMeasurement( System::Sensor::Element::IMON_LOAD );

    /*-------------------------------------------------------------------------
    Check range of the input and configuration values
    -------------------------------------------------------------------------*/
    bool can_engage = true;

    // Bounded by user configured input limits
    can_engage &= mbed_assert_continue_msg( vin_act <= vin_max, "OOB Vin %.2fV > %.2fV Cfg", vin_act, vin_max );
    can_engage &= mbed_assert_continue_msg( vin_act >= vin_min, "OOB Vin %.2fV < %.2fV Cfg", vin_act, vin_min );

    // Bounded by an attached battery/load that exceeds the system voltage output limits
    can_engage &= mbed_assert_continue_msg( vout_act <= vout_lim, "OOB Vout %.2fV > %.2fV Limit", vout_act, vout_lim );

    // Should not be drawing any meaningful current right now
    can_engage &= mbed_assert_continue_msg( iout_act < 100e-3f, "Iout %.2fA > 100mA", iout_act );

    // Target output voltage should be within the input range
    can_engage &= mbed_assert_continue_msg( vout_tgt <= vin_act, "Vout %.2fV > Vin %.2fV", vout_tgt, vin_act );

    // Target output voltage should be within the converter limits
    can_engage &= mbed_assert_continue_msg( vout_tgt <= vout_lim, "Vout %.2fV > %.2fV Limit", vout_tgt, vout_lim );

    // Target system output current should be within the converter limits
    can_engage &= mbed_assert_continue_msg( iout_tgt <= iout_lim, "Iout %.2fA > %.2fA Limit", iout_tgt, iout_lim );

    // Target phase current should be within the converter limits (6 phases)
    float phase_iout = iout_tgt / 6.0f;
    can_engage &=
        mbed_assert_continue_msg( phase_iout <= iout_phase_lim, "Phase Iout %.2fA > %.2fA Limit", phase_iout, iout_phase_lim );


    /*-------------------------------------------------------------------------
    *IMPORTANT* Really need to read any configuration data from the
    previous power cycle to ensure we don't boot up and immediately change
    output voltage/current limts. If a battery is connected and we reconfigure
    all willy nilly, it could be catastrophic.

    There should be a layered approach:
      1. If any of the following are true, return an invalid configuration:
        - Output voltage is present and no input voltage
        - Output voltage is higher than input voltage
        - No voltage on either the input or output
        - Any voltage is outside the expected operational range

      2. With a BMS attached, read and validate the configuration. If it's
         invalid, return false and do not boot. The BMS acts as a single
         point of truth to describe the connected storage system. It's also
         responsible for isolation from this buck converter.

      2. If the BMS is unavailable and no voltage is present on the output,
         we can assume there is no battery and power up to some sane values
         based on the input voltage. This will essentially be "free running"
         mode where we try and provide max power the solar panel can provide.

        2a. When free running, use the last programmed system limits for voltage
            and current and ramp to those points as a kind of "soft-start".
            This thing *is* still programmable via other interfaces after all.

    Log all boot decisions to the system log for later analysis.

    Update:
    - May want to always reset the LTC7871 to OFF unless the BMS commands it to
      turn on. This might be annoying if the system suddenly resets, but it
      ensures safety.

    - May want a button input to allow someone to manually turn the system on
      without the BMS. Would be interesting. We'd have to match output voltage
      already present or just simply refuse to boot if we see output voltage.
    -------------------------------------------------------------------------*/

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

    s_power_output_enabled = false;
  }


  void periodicProcessing()
  {
    switch( HW::LTC7871::getMode() )
    {
      /*-----------------------------------------------------------------------
      The power stage is enabled and running normally. Update the setpoints
      and monitor for faults.
      -----------------------------------------------------------------------*/
      case HW::LTC7871::DriverMode::ENABLED:
        if( ( s_voltage_request > 0.0f ) && ( s_voltage_request < App::PDI::getSystemVoltageOutputRatedLimit() ) )
        {
          App::PDI::setTargetSystemVoltageOutput( s_voltage_request );
          HW::LTC7871::setVoutRef( s_voltage_request );
          s_voltage_request = 0.0f;
        }

        if( ( s_current_request > 0.0f ) && ( s_current_request < App::PDI::getSystemCurrentOutputRatedLimit() ) )
        {
          App::PDI::setTargetSystemCurrentOutput( s_current_request );
          HW::LTC7871::setIoutRef( s_current_request );
          s_current_request = 0.0f;
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


  void setOutputVoltage( const float voltage )
  {
    s_voltage_request = voltage;
  }


  void setOutputCurrentLimit( const float current )
  {
    s_current_request = current;
  }
}    // namespace App::Power
