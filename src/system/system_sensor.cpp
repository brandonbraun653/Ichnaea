/******************************************************************************
 *  File Name:
 *    system_sensor.cpp
 *
 *  Description:
 *    Ichnaea system sensor measurement and data conversion routines
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "mbedutils/drivers/threading/thread.hpp"
#include <etl/algorithm.h>
#include <mbedutils/drivers/hardware/analog.hpp>
#include <mbedutils/logging.hpp>
#include <src/app/pdi/cal_output_current.hpp>
#include <src/bsp/board_map.hpp>
#include <src/hw/adc.hpp>
#include <src/hw/fan.hpp>
#include <src/hw/ltc7871.hpp>
#include <src/system/system_sensor.hpp>

namespace System::Sensor
{
  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/

  static float calc_thermistor_temp( const float vOut );
  static float get_ltc_avg_current( const LookupType lut );
  static float get_high_side_voltage( const LookupType lut );
  static float get_low_side_voltage( const LookupType lut );
  static float get_rp2040_temp( const LookupType lut );
  static float get_board_temp0( const LookupType lut );
  static float get_board_temp1( const LookupType lut );
  static float read_imon_load();
  static float get_imon_load( const LookupType lut );
  static float get_vmon_1v1( const LookupType lut );
  static float get_vmon_3v3( const LookupType lut );
  static float get_vmon_5v0( const LookupType lut );
  static float get_vmon_12v( const LookupType lut );

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    /*-------------------------------------------------------------------------
    Register sensor calibration PDI keys
    -------------------------------------------------------------------------*/
    App::PDI::pdi_register_key_cal_output_current();
  }


  float getMeasurement( const Element channel, const LookupType lut )
  {
    switch( channel )
    {
      case Element::RP2040_TEMP:
        return get_rp2040_temp( lut );

      case Element::BOARD_TEMP_0:
        return get_board_temp0( lut );

      case Element::BOARD_TEMP_1:
        return get_board_temp1( lut );

      case Element::IMON_LTC_AVG:
        return get_ltc_avg_current( lut );

      case Element::VMON_SOLAR_INPUT:
        return get_high_side_voltage( lut );

      case Element::VMON_LOAD:
        return get_low_side_voltage( lut );

      case Element::IMON_LOAD:
        return get_imon_load( lut );

      case Element::VMON_1V1:
        return get_vmon_1v1( lut );

      case Element::VMON_3V3:
        return get_vmon_3v3( lut );

      case Element::VMON_5V0:
        return get_vmon_5v0( lut );

      case Element::VMON_12V:
        return get_vmon_12v( lut );

      case Element::FAN_SPEED:
        return HW::FAN::getFanSpeed();

      default:
        mbed_assert_continue_msg( false, "Invalid sensor element: %d", static_cast<size_t>( channel ) );
        return 0.0f;
    }
  }


  void Calibration::calibrateImonNoLoadOffset()
  {
    constexpr size_t NUM_SAMPLES = 10;

    /*-------------------------------------------------------------------------
    Take a few samples and average them to get the offset value.
    -------------------------------------------------------------------------*/
    float offset = 0.0f;
    for( size_t i = 0; i < NUM_SAMPLES; i++ )
    {
      offset += read_imon_load();
      mb::thread::this_thread::sleep_for( 5 );
    }

    offset /= static_cast<float>( NUM_SAMPLES );

    /*-------------------------------------------------------------------------
    Update the calibration data
    -------------------------------------------------------------------------*/
    ichnaea_PDI_BasicCalibration calData;
    App::PDI::getCalOutputCurrent( calData );
    calData.offset = offset;
    App::PDI::setCalOutputCurrent( calData );
  }

  /*---------------------------------------------------------------------------
  Private Function Implementations
  ---------------------------------------------------------------------------*/

  static float calc_thermistor_temp( const float vOut )
  {
    auto ioConfig = BSP::getIOConfig();
    return Analog::calculateTempBeta( vOut, ioConfig.tmon_vdiv_input, ioConfig.tmon_beta_25C, ioConfig.tmon_vdiv_r1_fixed,
                                      ioConfig.tmon_vdiv_r2_thermistor, 25.0f );
  }


  static float get_ltc_avg_current( const LookupType lut )
  {
    static float s_cached_avg_current = 0.0f;
    if( lut == LookupType::CACHED )
    {
      return s_cached_avg_current;
    }

    float imon           = HW::ADC::getVoltage( HW::ADC::Channel::LTC_IMON );
    s_cached_avg_current = HW::LTC7871::getAverageOutputCurrent( imon );
    return s_cached_avg_current;
  }


  static float get_high_side_voltage( const LookupType lut )
  {
    static float s_cached_value = 0.0f;
    if( lut == LookupType::CACHED )
    {
      return s_cached_value;
    }

    auto  ioConfig = BSP::getIOConfig();
    float Vout     = HW::ADC::getVoltage( HW::ADC::Channel::HV_DC_SENSE );
    s_cached_value = Analog::calculateVoltageDividerInput( Vout, ioConfig.vmon_solar_vdiv_r1, ioConfig.vmon_solar_vdiv_r2 );
    return s_cached_value;
  }


  static float get_low_side_voltage( const LookupType lut )
  {
    static float s_cached_value = 0.0f;
    if( lut == LookupType::CACHED )
    {
      return s_cached_value;
    }

    auto  ioConfig = BSP::getIOConfig();
    float Vout     = HW::ADC::getVoltage( HW::ADC::Channel::LV_DC_SENSE );
    s_cached_value = Analog::calculateVoltageDividerInput( Vout, ioConfig.vmon_load_vdiv_r1, ioConfig.vmon_load_vdiv_r2 );
    return s_cached_value;
  }


  static float get_rp2040_temp( const LookupType lut )
  {
    static float s_cached_value = 0.0f;
    if( lut == LookupType::CACHED )
    {
      return s_cached_value;
    }

    /*-------------------------------------------------------------------------
    Taken from the RP2040 datasheet section 4.1.1.1
    -------------------------------------------------------------------------*/
    float temp     = HW::ADC::getVoltage( HW::ADC::Channel::RP2040_TEMP );
    s_cached_value = 27.0f - ( ( temp - 0.706f ) / 0.001721 );
    return s_cached_value;
  }


  static float get_board_temp0( const LookupType lut )
  {
    static float s_cached_value = 0.0f;
    if( lut == LookupType::CACHED )
    {
      return s_cached_value;
    }

    float vOut     = HW::ADC::getVoltage( HW::ADC::Channel::TEMP_SENSE_0 );
    s_cached_value = calc_thermistor_temp( vOut );
    return s_cached_value;
  }


  static float get_board_temp1( const LookupType lut )
  {
    static float s_cached_value = 0.0f;
    if( lut == LookupType::CACHED )
    {
      return s_cached_value;
    }

    float vOut     = HW::ADC::getVoltage( HW::ADC::Channel::TEMP_SENSE_1 );
    s_cached_value = calc_thermistor_temp( vOut );
    return s_cached_value;
  }


  /**
   * @brief Reads the IMON_LOAD channel and calculates the current in amps.
   *
   * @return float
   */
  static float read_imon_load()
  {
    auto  ioConfig = BSP::getIOConfig();
    float vOut     = HW::ADC::getVoltage( HW::ADC::Channel::IMON_LOAD );
    float vMsr     = Analog::calculateVoltageDividerInput( vOut, ioConfig.imon_load_vdiv_r1, ioConfig.imon_load_vdiv_r2 );
    return ( vMsr / ioConfig.imon_load_rsense ) * ( 1.0f / ioConfig.imon_load_opamp_gain );
  }


  /**
   * @brief Computes the calibrated IMON_LOAD current value in amps.
   *
   * @param lut Type of lookup to perform
   * @return float
   */
  static float get_imon_load( const LookupType lut )
  {
    static float s_cached_value = 0.0f;

    if( ( lut == LookupType::CACHED ) || ( BSP::getBoardRevision() < 2 ) )
    {
      return s_cached_value;
    }

    /*-------------------------------------------------------------------------
    Read the raw IMON_LOAD value
    -------------------------------------------------------------------------*/
    float iSenseRaw = read_imon_load();

    /*-------------------------------------------------------------------------
    Apply the calibration data to get the final current value
    -------------------------------------------------------------------------*/
    ichnaea_PDI_BasicCalibration calData;
    App::PDI::getCalOutputCurrent( calData );

    float iSenseCal = iSenseRaw * calData.gain - calData.offset;
    s_cached_value  = etl::clamp( iSenseCal, calData.valid_min, calData.valid_max );

    return s_cached_value;
  }


  static float get_vmon_1v1( const LookupType lut )
  {
    if( BSP::getBoardRevision() < 2 )
    {
      return 0.0f;
    }

    static float s_cached_value = 0.0f;
    if( lut == LookupType::CACHED )
    {
      return s_cached_value;
    }

    s_cached_value = HW::ADC::getVoltage( HW::ADC::Channel::VMON_1V1 );
    return s_cached_value;
  }


  static float get_vmon_3v3( const LookupType lut )
  {
    if( BSP::getBoardRevision() < 2 )
    {
      return 0.0f;
    }

    static float s_cached_value = 0.0f;
    if( lut == LookupType::CACHED )
    {
      return s_cached_value;
    }

    auto  ioConfig = BSP::getIOConfig();
    float vOut     = HW::ADC::getVoltage( HW::ADC::Channel::VMON_3V3 );
    s_cached_value = Analog::calculateVoltageDividerInput( vOut, ioConfig.vmon_3v3_vdiv_r1, ioConfig.vmon_3v3_vdiv_r2 );
    return s_cached_value;
  }


  static float get_vmon_5v0( const LookupType lut )
  {
    if( BSP::getBoardRevision() < 2 )
    {
      return 0.0f;
    }

    static float s_cached_value = 0.0f;
    if( lut == LookupType::CACHED )
    {
      return s_cached_value;
    }

    auto  ioConfig = BSP::getIOConfig();
    float vOut     = HW::ADC::getVoltage( HW::ADC::Channel::VMON_5V0 );
    s_cached_value = Analog::calculateVoltageDividerInput( vOut, ioConfig.vmon_5v0_vdiv_r1, ioConfig.vmon_5v0_vdiv_r2 );
    return s_cached_value;
  }


  static float get_vmon_12v( const LookupType lut )
  {
    if( BSP::getBoardRevision() < 2 )
    {
      return 0.0f;
    }

    static float s_cached_value = 0.0f;
    if( lut == LookupType::CACHED )
    {
      return s_cached_value;
    }

    auto  ioConfig = BSP::getIOConfig();
    float vOut     = HW::ADC::getVoltage( HW::ADC::Channel::VMON_12V );
    s_cached_value = Analog::calculateVoltageDividerInput( vOut, ioConfig.vmon_12v_vdiv_r1, ioConfig.vmon_12v_vdiv_r2 );
    return s_cached_value;
  }
}    // namespace System::Sensor
