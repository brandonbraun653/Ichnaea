/******************************************************************************
 *  File Name:
 *    app_monitor.cpp
 *
 *  Description:
 *    Function implementations for system monitoring actions.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cmath>
#include <etl/math.h>
#include <mbedutils/assert.hpp>
#include <mbedutils/database.hpp>
#include <mbedutils/interfaces/time_intf.hpp>
#include <mbedutils/logging.hpp>
#include <src/app/app_filter.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/app_power.hpp>
#include <src/app/pdi/config_max_system_voltage_input.hpp>
#include <src/app/pdi/config_max_temp_limit.hpp>
#include <src/app/pdi/config_min_system_voltage_input.hpp>
#include <src/app/pdi/config_min_temp_limit.hpp>
#include <src/app/pdi/mon_12v0_voltage.hpp>
#include <src/app/pdi/mon_1v1_voltage.hpp>
#include <src/app/pdi/mon_3v3_voltage.hpp>
#include <src/app/pdi/mon_5v0_voltage.hpp>
#include <src/app/pdi/mon_fan_speed.hpp>
#include <src/app/pdi/mon_input_voltage.hpp>
#include <src/app/pdi/mon_output_current.hpp>
#include <src/app/pdi/mon_output_voltage.hpp>
#include <src/app/pdi/mon_temperature.hpp>
#include <src/app/pdi/system_current_output_rated_limit.hpp>
#include <src/app/pdi/system_voltage_output_rated_limit.hpp>
#include <src/app/pdi/target_fan_speed_rpm.hpp>
#include <src/app/pdi/target_system_current_output.hpp>
#include <src/app/pdi/target_system_voltage_output.hpp>
#include <src/app/proto/ichnaea_pdi.pb.h>
#include <src/system/system_db.hpp>
#include <src/system/system_error.hpp>
#include <src/system/system_sensor.hpp>

namespace App::Monitor
{
  /*---------------------------------------------------------------------------
  Enumerations
  ---------------------------------------------------------------------------*/

  enum class RangeStateEvent : size_t
  {
    OUT_OF_RANGE,
    NO_CHANGE,
    IN_RANGE
  };

  /*---------------------------------------------------------------------------
  Structures
  ---------------------------------------------------------------------------*/

  struct MonitorState
  {
    bool                   valid;              /**< Flag indicating if the monitor has valid data */
    bool                   oor_latched;        /**< Flag indicating if the monitor is currently out-of-range */
    size_t                 oor_enter_time;     /**< Last time a fault was detected */
    size_t                 oor_exit_time;      /**< Last time a fault was cleared */
    size_t                 last_run_time;      /**< Last time the monitor was executed */
    size_t                 sample_rate_ms;     /**< Rate at which the monitor should sample data */
    size_t                 oor_enter_delay_ms; /**< Delay before entering out-of-range state */
    size_t                 oor_exit_delay_ms;  /**< Delay before exiting out-of-range state */
    App::Filter::IIRFilter filter;             /**< Filter to apply to the data */
    etl::string<32>        name;               /**< Name of the monitor */

    union PDIDependencies
    {
      struct InputVoltage
      {
        float min;
        float max;
      } input_voltage;

      struct LoadOvercurrent
      {
        float user_limit;
        float system_limit;
      } load_overcurrent;

      struct OutputVoltage
      {
        float user_target;
        float system_limit;
        float pct_error_limit;
      } output_voltage;

      struct GenericLowVoltageDC
      {
        float nominal_voltage;
        float pct_error_lim;
      } voltage;

      struct Temperature
      {
        float lower_limit;
        float upper_limit;
      } temperature;

      struct FanSpeed
      {
        float pct_error_lim;
        float target_speed;
      } fan_speed;
    } pdi;
  };
  using MonStateArray = etl::array<MonitorState, ( size_t )System::Sensor::Element::NUM_OPTIONS>;

  /*---------------------------------------------------------------------------
  Private Data
  ---------------------------------------------------------------------------*/

  static MonStateArray s_monitor_state;
  static bool          s_monitor_enabled;
  static bool          s_driver_initialized;

  /*---------------------------------------------------------------------------
  Private Function Declarations
  ---------------------------------------------------------------------------*/
  static bool            on_monitor_error( const Panic::ErrorCode &code );
  static RangeStateEvent apply_mon_range_event_hysteresis( MonitorState &state, const bool is_oor, const size_t sys_time_ms );
  static void            force_monitor_invalid( MonitorState &state );

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void driver_init()
  {
    /*-------------------------------------------------------------------------
    Reset module data
    -------------------------------------------------------------------------*/
    s_monitor_enabled    = false;
    s_driver_initialized = false;
    s_monitor_state.fill( {} );

    s_monitor_state[ ( size_t )System::Sensor::Element::RP2040_TEMP ].name      = "RP2040 Temp";
    s_monitor_state[ ( size_t )System::Sensor::Element::IMON_LTC_AVG ].name     = "LTC7871 Avg Current";
    s_monitor_state[ ( size_t )System::Sensor::Element::VMON_SOLAR_INPUT ].name = "Input Voltage";
    s_monitor_state[ ( size_t )System::Sensor::Element::IMON_LOAD ].name        = "Output Current";
    s_monitor_state[ ( size_t )System::Sensor::Element::VMON_LOAD ].name        = "Output Voltage";
    s_monitor_state[ ( size_t )System::Sensor::Element::VMON_1V1 ].name         = "1V1 Voltage";
    s_monitor_state[ ( size_t )System::Sensor::Element::VMON_3V3 ].name         = "3V3 Voltage";
    s_monitor_state[ ( size_t )System::Sensor::Element::VMON_5V0 ].name         = "5V0 Voltage";
    s_monitor_state[ ( size_t )System::Sensor::Element::VMON_12V ].name         = "12V0 Voltage";
    s_monitor_state[ ( size_t )System::Sensor::Element::BOARD_TEMP_0 ].name     = "Board Temp 0";
    s_monitor_state[ ( size_t )System::Sensor::Element::BOARD_TEMP_1 ].name     = "Board Temp 1";
    s_monitor_state[ ( size_t )System::Sensor::Element::FAN_SPEED ].name        = "Fan Speed";

    /*-------------------------------------------------------------------------
    Register the PDI keys for the monitoring system
    -------------------------------------------------------------------------*/
    App::PDI::pdi_register_key__input_voltage_raw();
    App::PDI::pdi_register_key__output_current_raw();
    App::PDI::pdi_register_key__output_voltage_raw();
    App::PDI::pdi_register_key__input_voltage_valid();
    App::PDI::pdi_register_key__output_current_valid();
    App::PDI::pdi_register_key__output_voltage_valid();
    App::PDI::pdi_register_key__1v1_voltage_valid();
    App::PDI::pdi_register_key__3v3_voltage_valid();
    App::PDI::pdi_register_key__5v0_voltage_valid();
    App::PDI::pdi_register_key__12v0_voltage_valid();
    App::PDI::pdi_register_key__temperature_valid();
    App::PDI::pdi_register_key__fan_speed_valid();
    App::PDI::pdi_register_key__1v1_voltage_filtered();
    App::PDI::pdi_register_key__3v3_voltage_filtered();
    App::PDI::pdi_register_key__5v0_voltage_filtered();
    App::PDI::pdi_register_key__12v0_voltage_filtered();
    App::PDI::pdi_register_key__input_voltage_filtered();
    App::PDI::pdi_register_key__output_current_filtered();
    App::PDI::pdi_register_key__output_voltage_filtered();
    App::PDI::pdi_register_key__temperature_filtered();
    App::PDI::pdi_register_key__fan_speed_filtered();
    App::PDI::pdi_register_key__config_mon_fan_speed_oor_entry_delay_ms();
    App::PDI::pdi_register_key__config_mon_fan_speed_oor_exit_delay_ms();
    App::PDI::pdi_register_key__config_mon_fan_speed_pct_error_oor_limit();
    App::PDI::pdi_register_key__config_mon_input_voltage_oor_entry_delay_ms();
    App::PDI::pdi_register_key__config_mon_input_voltage_oor_exit_delay_ms();
    App::PDI::pdi_register_key__config_mon_load_overcurrent_oor_entry_delay_ms();
    App::PDI::pdi_register_key__config_mon_load_overcurrent_oor_exit_delay_ms();
    App::PDI::pdi_register_key__config_mon_load_voltage_pct_error_oor_entry_delay_ms();
    App::PDI::pdi_register_key__config_mon_load_voltage_pct_error_oor_exit_delay_ms();
    App::PDI::pdi_register_key__config_mon_load_voltage_pct_error_oor_limit();
    App::PDI::pdi_register_key__config_mon_temperature_oor_entry_delay_ms();
    App::PDI::pdi_register_key__config_mon_temperature_oor_exit_delay_ms();

    /*-------------------------------------------------------------------------
    Register the error handler for to handle monitors going OOR
    -------------------------------------------------------------------------*/
    Panic::registerHandler( Panic::ErrorCode::ERR_MONITOR_12V0_OOR, Panic::ErrorCallback::create<on_monitor_error>() );
    Panic::registerHandler( Panic::ErrorCode::ERR_MONITOR_VIN_OOR, Panic::ErrorCallback::create<on_monitor_error>() );
    Panic::registerHandler( Panic::ErrorCode::ERR_MONITOR_VOUT_OOR, Panic::ErrorCallback::create<on_monitor_error>() );
    Panic::registerHandler( Panic::ErrorCode::ERR_MONITOR_IOUT_OOR, Panic::ErrorCallback::create<on_monitor_error>() );
    Panic::registerHandler( Panic::ErrorCode::ERR_MONITOR_TEMP_OOR, Panic::ErrorCallback::create<on_monitor_error>() );
    Panic::registerHandler( Panic::ErrorCode::ERR_MONITOR_FAN_SPEED_OOR, Panic::ErrorCallback::create<on_monitor_error>() );

    /*-------------------------------------------------------------------------
    Driver initialization sequence complete
    -------------------------------------------------------------------------*/
    s_driver_initialized = true;

    /*-------------------------------------------------------------------------
    Pull the latest information for each monitor's PDI dependencies
    -------------------------------------------------------------------------*/
    for( size_t i = 0; i < s_monitor_state.size(); i++ )
    {
      refreshPDIDependencies( ( System::Sensor::Element )i );
    }
  }


  void driver_deinit()
  {
    s_monitor_enabled    = false;
    s_driver_initialized = false;
  }


  void enable()
  {
    /*-------------------------------------------------------------------------
    Ensure we capture any invalid state immediately
    -------------------------------------------------------------------------*/
    for( auto &monitor : s_monitor_state )
    {
      force_monitor_invalid( monitor );
    }

    LOG_TRACE_IF( s_monitor_enabled, "System monitoring enabled" );
    s_monitor_enabled = true;
  }


  void disable()
  {
    LOG_TRACE_IF( s_monitor_enabled, "System monitoring disabled" );
    s_monitor_enabled = false;
  }


  void reset()
  {
    LOG_TRACE_IF( s_monitor_enabled, "System monitoring reset" );
    for( auto &monitor : s_monitor_state )
    {
      monitor.filter.reset();
    }
  }


  void refreshPDIDependencies( const System::Sensor::Element element )
  {
    /*-------------------------------------------------------------------------
    Input protection
    -------------------------------------------------------------------------*/
    if( !s_driver_initialized )
    {
      return;
    }

    const size_t idx = ( size_t )element;
    if( idx >= ( size_t )System::Sensor::Element::NUM_OPTIONS )
    {
      mbed_dbg_assert_continue_msg( false, "Invalid sensor element" );
      return;
    }

    /*-------------------------------------------------------------------------
    Configure the PDI dependencies for the given monitor
    -------------------------------------------------------------------------*/
    switch( element )
    {
      case System::Sensor::Element::VMON_SOLAR_INPUT:
        s_monitor_state[ idx ].filter.initialize( App::PDI::KEY_MON_FILTER_INPUT_VOLTAGE );
        s_monitor_state[ idx ].sample_rate_ms        = App::PDI::getMonFilterInputVoltage().sampleRateMs;
        s_monitor_state[ idx ].pdi.input_voltage.min = App::PDI::getConfigMinSystemVoltageInput();
        s_monitor_state[ idx ].pdi.input_voltage.max = App::PDI::getConfigMaxSystemVoltageInput();
        s_monitor_state[ idx ].oor_enter_delay_ms    = App::PDI::getMonInputVoltageOOREntryDelayMS();
        s_monitor_state[ idx ].oor_exit_delay_ms     = App::PDI::getMonInputVoltageOORExitDelayMS();

        force_monitor_invalid( s_monitor_state[ idx ] );
        break;

      case System::Sensor::Element::IMON_LOAD:
        s_monitor_state[ idx ].filter.initialize( App::PDI::KEY_MON_FILTER_OUTPUT_CURRENT );
        s_monitor_state[ idx ].sample_rate_ms                    = App::PDI::getMonFilterOutputCurrent().sampleRateMs;
        s_monitor_state[ idx ].pdi.load_overcurrent.user_limit   = App::PDI::getTargetSystemCurrentOutput();
        s_monitor_state[ idx ].pdi.load_overcurrent.system_limit = App::PDI::getSystemCurrentOutputRatedLimit();
        s_monitor_state[ idx ].oor_enter_delay_ms                = App::PDI::getMonLoadOvercurrentOOREntryDelayMS();
        s_monitor_state[ idx ].oor_exit_delay_ms                 = App::PDI::getMonLoadOvercurrentOORExitDelayMS();

        force_monitor_invalid( s_monitor_state[ idx ] );
        break;

      case System::Sensor::Element::VMON_LOAD:
        s_monitor_state[ idx ].filter.initialize( App::PDI::KEY_MON_FILTER_OUTPUT_VOLTAGE );
        s_monitor_state[ idx ].sample_rate_ms                     = App::PDI::getMonFilterOutputVoltage().sampleRateMs;
        s_monitor_state[ idx ].pdi.output_voltage.user_target     = App::PDI::getTargetSystemVoltageOutput();
        s_monitor_state[ idx ].pdi.output_voltage.system_limit    = App::PDI::getSystemVoltageOutputRatedLimit();
        s_monitor_state[ idx ].pdi.output_voltage.pct_error_limit = App::PDI::getMonLoadVoltagePctErrorOORLimit();
        s_monitor_state[ idx ].oor_enter_delay_ms                 = App::PDI::getMonLoadVoltagePctErrorOOREntryDelayMS();
        s_monitor_state[ idx ].oor_exit_delay_ms                  = App::PDI::getMonLoadVoltagePctErrorOORExitDelayMS();

        force_monitor_invalid( s_monitor_state[ idx ] );
        break;

      case System::Sensor::Element::VMON_1V1:
        s_monitor_state[ idx ].filter.initialize( App::PDI::KEY_MON_FILTER_1V1_VOLTAGE );
        s_monitor_state[ idx ].sample_rate_ms              = App::PDI::getMonFilter1V1Voltage().sampleRateMs;
        s_monitor_state[ idx ].pdi.voltage.nominal_voltage = 1.1f;
        s_monitor_state[ idx ].pdi.voltage.pct_error_lim   = 0.05f;
        s_monitor_state[ idx ].oor_enter_delay_ms          = 500;
        s_monitor_state[ idx ].oor_exit_delay_ms           = 10;

        force_monitor_invalid( s_monitor_state[ idx ] );
        break;

      case System::Sensor::Element::VMON_3V3:
        s_monitor_state[ idx ].filter.initialize( App::PDI::KEY_MON_FILTER_3V3_VOLTAGE );
        s_monitor_state[ idx ].sample_rate_ms              = App::PDI::getMonFilter3V3Voltage().sampleRateMs;
        s_monitor_state[ idx ].pdi.voltage.nominal_voltage = 3.3f;
        s_monitor_state[ idx ].pdi.voltage.pct_error_lim   = 0.05f;
        s_monitor_state[ idx ].oor_enter_delay_ms          = 500;
        s_monitor_state[ idx ].oor_exit_delay_ms           = 10;

        force_monitor_invalid( s_monitor_state[ idx ] );
        break;

      case System::Sensor::Element::VMON_5V0:
        s_monitor_state[ idx ].filter.initialize( App::PDI::KEY_MON_FILTER_5V0_VOLTAGE );
        s_monitor_state[ idx ].sample_rate_ms              = App::PDI::getMonFilter5V0Voltage().sampleRateMs;
        s_monitor_state[ idx ].pdi.voltage.nominal_voltage = 5.0f;
        s_monitor_state[ idx ].pdi.voltage.pct_error_lim   = 0.05f;
        s_monitor_state[ idx ].oor_enter_delay_ms          = 500;
        s_monitor_state[ idx ].oor_exit_delay_ms           = 10;

        force_monitor_invalid( s_monitor_state[ idx ] );
        break;

      case System::Sensor::Element::VMON_12V:
        s_monitor_state[ idx ].filter.initialize( App::PDI::KEY_MON_FILTER_12V0_VOLTAGE );
        s_monitor_state[ idx ].sample_rate_ms              = App::PDI::getMonFilter12V0Voltage().sampleRateMs;
        s_monitor_state[ idx ].pdi.voltage.nominal_voltage = 12.0f;
        s_monitor_state[ idx ].pdi.voltage.pct_error_lim   = 0.05f;
        s_monitor_state[ idx ].oor_enter_delay_ms          = 2000;    // Must account for LTC startup time from near zero
        s_monitor_state[ idx ].oor_exit_delay_ms           = 10;

        force_monitor_invalid( s_monitor_state[ idx ] );
        break;

      case System::Sensor::Element::BOARD_TEMP_0:
      case System::Sensor::Element::BOARD_TEMP_1:
        s_monitor_state[ idx ].filter.initialize( App::PDI::KEY_MON_FILTER_TEMPERATURE );
        s_monitor_state[ idx ].sample_rate_ms              = App::PDI::getMonFilterTemperature().sampleRateMs;
        s_monitor_state[ idx ].pdi.temperature.lower_limit = App::PDI::getConfigMinTempLimit();
        s_monitor_state[ idx ].pdi.temperature.upper_limit = App::PDI::getConfigMaxTempLimit();
        s_monitor_state[ idx ].oor_enter_delay_ms          = App::PDI::getMonTemperatureOOREntryDelayMS();
        s_monitor_state[ idx ].oor_exit_delay_ms           = App::PDI::getMonTemperatureOORExitDelayMS();

        force_monitor_invalid( s_monitor_state[ idx ] );
        break;

      case System::Sensor::Element::FAN_SPEED:
        s_monitor_state[ idx ].filter.initialize( App::PDI::KEY_MON_FILTER_FAN_SPEED );
        s_monitor_state[ idx ].sample_rate_ms              = App::PDI::getMonFilterFanSpeed().sampleRateMs;
        s_monitor_state[ idx ].pdi.fan_speed.pct_error_lim = App::PDI::getMonFanSpeedPctErrorOORLimit();
        s_monitor_state[ idx ].pdi.fan_speed.target_speed  = App::PDI::getTargetFanSpeedRPM();
        s_monitor_state[ idx ].oor_enter_delay_ms          = App::PDI::getMonFanSpeedOOREntryDelayMS();
        s_monitor_state[ idx ].oor_exit_delay_ms           = App::PDI::getMonFanSpeedOORExitDelayMS();

        force_monitor_invalid( s_monitor_state[ idx ] );
        break;

      default:
        break;
    }

    /*-------------------------------------------------------------------------
    Reset the monitor state so that it will re-acquire validity status
    -------------------------------------------------------------------------*/
    force_monitor_invalid( s_monitor_state[ idx ] );
  }

  void monitorInputVoltage()
  {
    static MonitorState *s = &s_monitor_state[ ( size_t )System::Sensor::Element::VMON_SOLAR_INPUT ];

    /*-------------------------------------------------------------------------
    Ensure we don't sample too quickly
    -------------------------------------------------------------------------*/
    size_t currentTime = mb::time::millis();
    if( ( currentTime - s->last_run_time ) <= s->sample_rate_ms )
    {
      return;
    }

    s->last_run_time = currentTime;

    /*-------------------------------------------------------------------------
    Sample the input voltage
    -------------------------------------------------------------------------*/
    float raw_voltage   = System::Sensor::getMeasurement( System::Sensor::Element::VMON_SOLAR_INPUT );
    float filtered_data = s->filter.apply( raw_voltage );

    /*-------------------------------------------------------------------------
    Update the PDI database with the new data
    -------------------------------------------------------------------------*/
    App::PDI::setMonInputVoltageRaw( raw_voltage );
    App::PDI::setMonInputVoltageFiltered( filtered_data );

    /*-------------------------------------------------------------------------
    Take action on the filtered data
    -------------------------------------------------------------------------*/
    bool data_oor_min = filtered_data < s->pdi.input_voltage.min;
    bool data_oor_max = filtered_data > s->pdi.input_voltage.max;

    switch( apply_mon_range_event_hysteresis( *s, data_oor_max || data_oor_min, currentTime ) )
    {
      case RangeStateEvent::OUT_OF_RANGE:
        PDI::setMonInputVoltageValid( false );
        LOG_WARN_IF( s_monitor_enabled, "%s Invalid: %.2fV", s->name.c_str(), filtered_data );

        if( s_monitor_enabled )
        {
          mbed_assert_continue_msg( false, "%s OOR: %.2fV", s->name.c_str(), filtered_data );
          Panic::throwError( Panic::ErrorCode::ERR_MONITOR_VIN_OOR );
        }
        break;

      case RangeStateEvent::IN_RANGE:
        LOG_TRACE_IF( s_monitor_enabled, "%s Valid: %.2fV", s->name.c_str(), filtered_data );
        PDI::setMonInputVoltageValid( true );
        break;

      case RangeStateEvent::NO_CHANGE:
      default:
        // No action to take
        break;
    }
  }

  void monitorOutputCurrent()
  {
    static MonitorState *s = &s_monitor_state[ ( size_t )System::Sensor::Element::IMON_LOAD ];

    /*-------------------------------------------------------------------------
    Ensure we don't sample too quickly
    -------------------------------------------------------------------------*/
    size_t currentTime = mb::time::millis();
    if( ( currentTime - s->last_run_time ) <= s->sample_rate_ms )
    {
      return;
    }

    s->last_run_time = currentTime;

    /*-------------------------------------------------------------------------
    Sample the output current
    -------------------------------------------------------------------------*/
    float raw_current   = System::Sensor::getMeasurement( System::Sensor::Element::IMON_LOAD );
    float filtered_data = s->filter.apply( raw_current );

    /*-------------------------------------------------------------------------
    Update the PDI database with the new data
    -------------------------------------------------------------------------*/
    App::PDI::setMonOutputCurrentRaw( raw_current );
    App::PDI::setMonOutputCurrentFiltered( filtered_data );

    /*-------------------------------------------------------------------------
    Take action on the filtered data
    -------------------------------------------------------------------------*/
    bool data_oor =
        ( filtered_data > s->pdi.load_overcurrent.user_limit ) || ( filtered_data > s->pdi.load_overcurrent.system_limit );

    switch( apply_mon_range_event_hysteresis( *s, data_oor, currentTime ) )
    {
      case RangeStateEvent::OUT_OF_RANGE:
        PDI::setMonOutputCurrentValid( false );
        LOG_WARN_IF( s_monitor_enabled, "%s Invalid: %.2fA", s->name.c_str(), filtered_data );

        if( s_monitor_enabled )
        {
          mbed_assert_continue_msg( false, "%s OOR: %.2fA", s->name.c_str(), filtered_data );
          Panic::throwError( Panic::ErrorCode::ERR_MONITOR_IOUT_OOR );
        }
        break;

      case RangeStateEvent::IN_RANGE:
        LOG_TRACE_IF( s_monitor_enabled, "%s Valid: %.2fA", s->name.c_str(), filtered_data );
        PDI::setMonOutputCurrentValid( true );
        break;

      case RangeStateEvent::NO_CHANGE:
      default:
        // No action to take
        break;
    }
  }

  void monitorOutputVoltage()
  {
    static MonitorState *s = &s_monitor_state[ ( size_t )System::Sensor::Element::VMON_LOAD ];

    /*-------------------------------------------------------------------------
    Ensure we don't sample too quickly
    -------------------------------------------------------------------------*/
    size_t currentTime = mb::time::millis();
    if( ( currentTime - s->last_run_time ) <= s->sample_rate_ms )
    {
      return;
    }

    s->last_run_time = currentTime;

    /*-------------------------------------------------------------------------
    Sample the output voltage
    -------------------------------------------------------------------------*/
    float raw_voltage   = System::Sensor::getMeasurement( System::Sensor::Element::VMON_LOAD );
    float filtered_data = s->filter.apply( raw_voltage );

    /*-------------------------------------------------------------------------
    Update the PDI database with the new data
    -------------------------------------------------------------------------*/
    App::PDI::setMonOutputVoltageRaw( raw_voltage );
    App::PDI::setMonOutputVoltageFiltered( filtered_data );

    /*-------------------------------------------------------------------------
    Take action on the filtered data
    -------------------------------------------------------------------------*/
    float pct_error     = ( s->pdi.output_voltage.user_target != 0.0f )
                              ? fabs( ( filtered_data - s->pdi.output_voltage.user_target ) / s->pdi.output_voltage.user_target )
                              : fabs( filtered_data );
    bool  pct_error_oor = pct_error > s->pdi.output_voltage.pct_error_limit;
    bool  vout_max_oor  = filtered_data > s->pdi.output_voltage.system_limit;

    switch( apply_mon_range_event_hysteresis( *s, pct_error_oor || vout_max_oor, currentTime ) )
    {
      case RangeStateEvent::OUT_OF_RANGE:
        PDI::setMonOutputVoltageValid( false );
        if( s_monitor_enabled )
        {
          mbed_assert_continue_msg( !pct_error_oor, "%s exceeded %.2f%% error, Exp: %.2fV, Act: %.2fV", s->name.c_str(),
                                    s->pdi.output_voltage.pct_error_limit * 100.0f, s->pdi.output_voltage.user_target,
                                    filtered_data );

          mbed_assert_continue_msg( !vout_max_oor, "%s exceeded max limit: %.2fV, Act: %.2fV", s->name.c_str(),
                                    s->pdi.output_voltage.system_limit, filtered_data );
          Panic::throwError( Panic::ErrorCode::ERR_MONITOR_VOUT_OOR );
        }
        break;

      case RangeStateEvent::IN_RANGE:
        LOG_TRACE_IF( s_monitor_enabled, "%s Valid: %.2fV", s->name.c_str(), filtered_data );
        PDI::setMonOutputVoltageValid( true );
        break;

      case RangeStateEvent::NO_CHANGE:
      default:
        // No action to take
        break;
    }
  }

  void monitor1V1Voltage()
  {
    static MonitorState *s = &s_monitor_state[ ( size_t )System::Sensor::Element::VMON_1V1 ];

    /*-------------------------------------------------------------------------
    Ensure we don't sample too quickly
    -------------------------------------------------------------------------*/
    size_t currentTime = mb::time::millis();
    if( ( currentTime - s->last_run_time ) <= s->sample_rate_ms )
    {
      return;
    }

    s->last_run_time = currentTime;

    /*-------------------------------------------------------------------------
    Sample the 1V1 voltage
    -------------------------------------------------------------------------*/
    float raw_voltage   = System::Sensor::getMeasurement( System::Sensor::Element::VMON_1V1 );
    float filtered_data = s->filter.apply( raw_voltage );

    /*-------------------------------------------------------------------------
    Update the PDI database with the new data
    -------------------------------------------------------------------------*/
    App::PDI::setMon1V1VoltageFiltered( filtered_data );

    /*-------------------------------------------------------------------------
    Take action on the filtered data
    -------------------------------------------------------------------------*/
    float pct_error = fabs( ( filtered_data - s->pdi.voltage.nominal_voltage ) / s->pdi.voltage.nominal_voltage );
    bool  data_oor  = pct_error > s->pdi.voltage.pct_error_lim;

    switch( apply_mon_range_event_hysteresis( *s, data_oor, currentTime ) )
    {
      case RangeStateEvent::OUT_OF_RANGE:
        PDI::setMon1V1VoltageValid( false );
        LOG_WARN_IF( s_monitor_enabled, "%s Invalid: %.2fV", s->name.c_str(), filtered_data );

        if( s_monitor_enabled )
        {
          mbed_assert_continue_msg( false, "%s OOR: %.2fV", s->name.c_str(), filtered_data );
        }
        break;

      case RangeStateEvent::IN_RANGE:
        LOG_TRACE_IF( s_monitor_enabled, "%s Valid: %.2fV", s->name.c_str(), filtered_data );
        PDI::setMon1V1VoltageValid( true );
        break;

      case RangeStateEvent::NO_CHANGE:
      default:
        // No action to take
        break;
    }
  }

  void monitor3V3Voltage()
  {
    static MonitorState *s = &s_monitor_state[ ( size_t )System::Sensor::Element::VMON_3V3 ];

    /*-------------------------------------------------------------------------
    Ensure we don't sample too quickly
    -------------------------------------------------------------------------*/
    size_t currentTime = mb::time::millis();
    if( ( currentTime - s->last_run_time ) <= s->sample_rate_ms )
    {
      return;
    }

    s->last_run_time = currentTime;

    /*-------------------------------------------------------------------------
    Sample the 3V3 voltage
    -------------------------------------------------------------------------*/
    float raw_voltage   = System::Sensor::getMeasurement( System::Sensor::Element::VMON_3V3 );
    float filtered_data = s->filter.apply( raw_voltage );

    /*-------------------------------------------------------------------------
    Update the PDI database with the new data
    -------------------------------------------------------------------------*/
    App::PDI::setMon3V3VoltageFiltered( filtered_data );

    /*-------------------------------------------------------------------------
    Take action on the filtered data
    -------------------------------------------------------------------------*/
    float pct_error = fabs( ( filtered_data - s->pdi.voltage.nominal_voltage ) / s->pdi.voltage.nominal_voltage );
    bool  data_oor  = pct_error > s->pdi.voltage.pct_error_lim;

    switch( apply_mon_range_event_hysteresis( *s, data_oor, currentTime ) )
    {
      case RangeStateEvent::OUT_OF_RANGE:
        PDI::setMon3V3VoltageValid( false );
        LOG_WARN_IF( s_monitor_enabled, "%s Invalid: %.2fV", s->name.c_str(), filtered_data );

        if( s_monitor_enabled )
        {
          mbed_assert_continue_msg( false, "%s OOR: %.2fV", s->name.c_str(), filtered_data );
        }
        break;

      case RangeStateEvent::IN_RANGE:
        LOG_TRACE_IF( s_monitor_enabled, "%s Valid: %.2fV", s->name.c_str(), filtered_data );
        PDI::setMon3V3VoltageValid( true );
        break;

      case RangeStateEvent::NO_CHANGE:
      default:
        // No action to take
        break;
    }
  }

  void monitor5V0Voltage()
  {
    static MonitorState *s = &s_monitor_state[ ( size_t )System::Sensor::Element::VMON_5V0 ];

    /*-------------------------------------------------------------------------
    Ensure we don't sample too quickly
    -------------------------------------------------------------------------*/
    size_t currentTime = mb::time::millis();
    if( ( currentTime - s->last_run_time ) <= s->sample_rate_ms )
    {
      return;
    }

    s->last_run_time = currentTime;

    /*-------------------------------------------------------------------------
    Sample the 5V0 voltage
    -------------------------------------------------------------------------*/
    float raw_voltage   = System::Sensor::getMeasurement( System::Sensor::Element::VMON_5V0 );
    float filtered_data = s->filter.apply( raw_voltage );

    /*-------------------------------------------------------------------------
    Update the PDI database with the new data
    -------------------------------------------------------------------------*/
    App::PDI::setMon5V0VoltageFiltered( filtered_data );

    /*-------------------------------------------------------------------------
    Take action on the filtered data
    -------------------------------------------------------------------------*/
    float pct_error = fabs( ( filtered_data - s->pdi.voltage.nominal_voltage ) / s->pdi.voltage.nominal_voltage );
    bool  data_oor  = pct_error > s->pdi.voltage.pct_error_lim;

    switch( apply_mon_range_event_hysteresis( *s, data_oor, currentTime ) )
    {
      case RangeStateEvent::OUT_OF_RANGE:
        PDI::setMon5V0VoltageValid( false );
        LOG_WARN_IF( s_monitor_enabled, "%s Invalid: %.2fV", s->name.c_str(), filtered_data );

        if( s_monitor_enabled )
        {
          mbed_assert_continue_msg( false, "%s OOR: %.2fV", s->name.c_str(), filtered_data );
        }
        break;

      case RangeStateEvent::IN_RANGE:
        LOG_TRACE_IF( s_monitor_enabled, "%s Valid: %.2fV", s->name.c_str(), filtered_data );
        PDI::setMon5V0VoltageValid( true );
        break;

      case RangeStateEvent::NO_CHANGE:
      default:
        // No action to take
        break;
    }
  }

  void monitor12V0Voltage()
  {
    static MonitorState *s = &s_monitor_state[ ( size_t )System::Sensor::Element::VMON_12V ];

    /*-------------------------------------------------------------------------
    Ensure we don't sample too quickly
    -------------------------------------------------------------------------*/
    size_t currentTime = mb::time::millis();
    if( ( currentTime - s->last_run_time ) <= s->sample_rate_ms )
    {
      return;
    }

    s->last_run_time = currentTime;

    /*-------------------------------------------------------------------------
    Sample the 12V0 voltage
    -------------------------------------------------------------------------*/
    float raw_voltage   = System::Sensor::getMeasurement( System::Sensor::Element::VMON_12V );
    float filtered_data = s->filter.apply( raw_voltage );

    /*-------------------------------------------------------------------------
    Update the PDI database with the new data
    -------------------------------------------------------------------------*/
    App::PDI::setMon12V0VoltageFiltered( filtered_data );

    /*-------------------------------------------------------------------------
    Take action on the filtered data
    -------------------------------------------------------------------------*/
    float pct_error = fabs( ( filtered_data - s->pdi.voltage.nominal_voltage ) / s->pdi.voltage.nominal_voltage );
    bool  data_oor  = pct_error > s->pdi.voltage.pct_error_lim;

    switch( apply_mon_range_event_hysteresis( *s, data_oor, currentTime ) )
    {
      case RangeStateEvent::OUT_OF_RANGE:
        PDI::setMon12V0VoltageValid( false );
        LOG_WARN_IF( s_monitor_enabled, "%s Invalid: %.2fV", s->name.c_str(), filtered_data );

        if( s_monitor_enabled )
        {
          mbed_assert_continue_msg( false, "%s OOR: %.2fV", s->name.c_str(), filtered_data );
          Panic::throwError( Panic::ErrorCode::ERR_MONITOR_12V0_OOR );
        }
        break;

      case RangeStateEvent::IN_RANGE:
        LOG_TRACE_IF( s_monitor_enabled, "%s Valid: %.2fV", s->name.c_str(), filtered_data );
        PDI::setMon12V0VoltageValid( true );
        break;

      case RangeStateEvent::NO_CHANGE:
      default:
        // No action to take
        break;
    }
  }

  void monitorTemperature()
  {
    static MonitorState *s = &s_monitor_state[ ( size_t )System::Sensor::Element::BOARD_TEMP_0 ];

    /*-------------------------------------------------------------------------
    Ensure we don't sample too quickly
    -------------------------------------------------------------------------*/
    size_t currentTime = mb::time::millis();
    if( ( currentTime - s->last_run_time ) <= s->sample_rate_ms )
    {
      return;
    }

    s->last_run_time = currentTime;

    /*-------------------------------------------------------------------------
    Sample the temperature
    -------------------------------------------------------------------------*/
    float raw_temp0     = System::Sensor::getMeasurement( System::Sensor::Element::BOARD_TEMP_0 );
    float raw_temp1     = System::Sensor::getMeasurement( System::Sensor::Element::BOARD_TEMP_1 );
    float avg_temp      = ( raw_temp0 + raw_temp1 ) / 2.0f;
    float filtered_data = s->filter.apply( avg_temp );

    mbed_dbg_assert( !etl::is_nan( filtered_data ) );

    /*-------------------------------------------------------------------------
    Update the PDI database with the new data
    -------------------------------------------------------------------------*/
    App::PDI::setMonTemperatureFiltered( filtered_data );

    /*-------------------------------------------------------------------------
    Take action on the filtered data
    -------------------------------------------------------------------------*/
    bool data_oor = ( filtered_data > s->pdi.temperature.upper_limit ) || ( filtered_data < s->pdi.temperature.lower_limit );

    switch( apply_mon_range_event_hysteresis( *s, data_oor, currentTime ) )
    {
      case RangeStateEvent::OUT_OF_RANGE:
        PDI::setMonTemperatureValid( false );
        LOG_WARN_IF( s_monitor_enabled, "%s Invalid: %.2fC", s->name.c_str(), filtered_data );

        if( s_monitor_enabled )
        {
          mbed_assert_continue_msg( false, "%s OOR: %.2fC", s->name.c_str(), filtered_data );
          Panic::throwError( Panic::ErrorCode::ERR_MONITOR_TEMP_OOR );
        }
        break;

      case RangeStateEvent::IN_RANGE:
        LOG_TRACE_IF( s_monitor_enabled, "%s Valid: %.2fC", s->name.c_str(), filtered_data );
        PDI::setMonTemperatureValid( true );
        break;

      case RangeStateEvent::NO_CHANGE:
      default:
        // No action to take
        break;
    }
  }

  void monitorFanSpeed()
  {
    static size_t        begin_invalid_state = 0;
    static MonitorState *s                   = &s_monitor_state[ ( size_t )System::Sensor::Element::FAN_SPEED ];

    /*-------------------------------------------------------------------------
    Ensure we don't sample too quickly
    -------------------------------------------------------------------------*/
    size_t currentTime = mb::time::millis();
    if( ( currentTime - s->last_run_time ) <= s->sample_rate_ms )
    {
      return;
    }

    s->last_run_time = currentTime;

    /*-------------------------------------------------------------------------
    Sample the fan speed
    -------------------------------------------------------------------------*/
    float raw_speed     = System::Sensor::getMeasurement( System::Sensor::Element::FAN_SPEED );
    float filtered_data = s->filter.apply( raw_speed );

    /*-------------------------------------------------------------------------
    Update the PDI database with the new data
    -------------------------------------------------------------------------*/
    App::PDI::setMonFanSpeedFiltered( filtered_data );

    /*-------------------------------------------------------------------------
    Take action on the filtered data
    -------------------------------------------------------------------------*/
    float pct_error = fabs( ( filtered_data - s->pdi.fan_speed.target_speed ) / s->pdi.fan_speed.target_speed );
    bool  data_oor  = pct_error > s->pdi.fan_speed.pct_error_lim;

    switch( apply_mon_range_event_hysteresis( *s, data_oor, currentTime ) )
    {
      case RangeStateEvent::OUT_OF_RANGE:
        PDI::setMonFanSpeedValid( false );
        LOG_WARN_IF( s_monitor_enabled, "%s Invalid: %.2f RPM", s->name.c_str(), filtered_data );

        if( s_monitor_enabled )
        {
          mbed_assert_continue_msg( false, "%s OOR: %.2f RPM", s->name.c_str(), filtered_data );
          begin_invalid_state = currentTime;
        }
        break;

      case RangeStateEvent::IN_RANGE:
        LOG_TRACE_IF( s_monitor_enabled, "%s Valid: %.2f RPM", s->name.c_str(), filtered_data );
        PDI::setMonFanSpeedValid( true );
        begin_invalid_state = 0;
        break;

      case RangeStateEvent::NO_CHANGE:
      default:
        // No action to take
        break;
    }

    /*-------------------------------------------------------------------------
    If the fan speed has been invalid for too long, panic. We need the cooling!
    -------------------------------------------------------------------------*/
    if( s_monitor_enabled && ( begin_invalid_state != 0 ) && ( currentTime - begin_invalid_state ) >= 10'000 )
    {
      Panic::throwError( Panic::ErrorCode::ERR_MONITOR_FAN_SPEED_OOR );
    }
  }


  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/

  static bool on_monitor_error( const Panic::ErrorCode &code )
  {
    /*-------------------------------------------------------------------------
    Ensure we only respond to monitor-related errors
    -------------------------------------------------------------------------*/
    if( ( code < Panic::ErrorCode::_ERR_MONITOR_START ) || ( code >= Panic::ErrorCode::_ERR_MONITOR_END ) )
    {
      mbed_dbg_assert_continue_msg( false, "Invalid monitor error code: %d", code );
      return false;
    }

    /*-------------------------------------------------------------------------
    Put the system into a safe state
    -------------------------------------------------------------------------*/
    LOG_WARN( "Safe-ing system due to monitor error: %d", code );
    App::Power::disengageOutput();
    return true;
  }

  /**
   * @brief Computes IR/OOR state change events for a given monitor.
   *
   * This uses the monitor's internal state to detect when an immediate OOR
   * event should transition into/out of a latched OOR state. There allows
   * configurable behavior for individual monitors.
   *
   * @param state The monitor's state
   * @param is_oor Immediate OOR state
   * @param sys_time_ms Current system time in milliseconds
   * @return Which event just occured
   */
  static RangeStateEvent apply_mon_range_event_hysteresis( MonitorState &state, const bool is_oor, const size_t sys_time_ms )
  {
    RangeStateEvent event = RangeStateEvent::NO_CHANGE;

    if( is_oor )    // Transition from Valid -> OOR, or OOR -> OOR
    {
      /*-----------------------------------------------------------------------
      Reset state conditions that always hold true at this point
      -----------------------------------------------------------------------*/
      state.oor_exit_time = 0;

      /*-----------------------------------------------------------------------
      Start the state timer if this is the first detection
      -----------------------------------------------------------------------*/
      if( state.oor_enter_time == 0 )
      {
        state.oor_enter_time = sys_time_ms;
      }

      /*-----------------------------------------------------------------------
      Check if enough time has elapsed to trigger the OOR state change
      -----------------------------------------------------------------------*/
      bool oor_window_entered = ( sys_time_ms - state.oor_enter_time ) >= state.oor_enter_delay_ms;

      if( state.valid && oor_window_entered )
      {
        state.valid       = false;
        state.oor_latched = true;
        event             = RangeStateEvent::OUT_OF_RANGE;
      }
      else if( !state.valid && !state.oor_latched && oor_window_entered )
      {
        state.oor_latched = true;
        event             = RangeStateEvent::OUT_OF_RANGE;
      }
    }
    else if( ( is_oor == false ) && ( state.valid == false ) )    // Transition from OOR -> Valid
    {
      /*-----------------------------------------------------------------------
      Reset state conditions that always hold true at this point
      -----------------------------------------------------------------------*/
      state.oor_enter_time = 0;

      /*-----------------------------------------------------------------------
      Start the state timer if this is the first detection
      -----------------------------------------------------------------------*/
      if( state.oor_exit_time == 0 )
      {
        state.oor_exit_time = sys_time_ms;
      }

      /*-----------------------------------------------------------------------
      Check if enough time has elapsed to trigger the OOR state change
      -----------------------------------------------------------------------*/
      size_t time_since_detection = sys_time_ms - state.oor_exit_time;
      if( time_since_detection >= state.oor_exit_delay_ms )
      {
        state.valid       = true;
        state.oor_latched = false;    // Unlatch the OOR condition
        event             = RangeStateEvent::IN_RANGE;
      }
    }
    else    // No change
    {
      /*-----------------------------------------------------------------------
      Ensure timers are ready to go
      -----------------------------------------------------------------------*/
      state.oor_enter_time = 0;
      state.oor_exit_time  = 0;
    }

    return event;
  }

  static void force_monitor_invalid( MonitorState &state )
  {
    state.valid          = false;
    state.oor_latched    = false;
    state.oor_enter_time = 0;
    state.oor_exit_time  = 0;
    LOG_TRACE_IF( s_monitor_enabled, "%s monitor reset", state.name.c_str() );
  }

}    // namespace App::Monitor
