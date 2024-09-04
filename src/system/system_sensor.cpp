/******************************************************************************
 *  File Name:
 *    system_sensor.cpp
 *
 *  Description:
 *    Ichnaea system sensor measurement routines
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <algorithm>
#include "src/bsp/board_map.hpp"
#include "src/system/system_sensor.hpp"
#include "src/hw/ltc7871.hpp"
#include "src/hw/adc.hpp"
#include <mbedutils/logging.hpp>
#include <mbedutils/drivers/hardware/analog.hpp>

namespace Sensor
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
  static float get_imon_batt( const LookupType lut );
  static float get_vmon_1v1( const LookupType lut );
  static float get_vmon_3v3( const LookupType lut );
  static float get_vmon_5v0( const LookupType lut );
  static float get_vmon_12v( const LookupType lut );

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

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

      case Element::VMON_BATT_OUTPUT:
        return get_low_side_voltage( lut );

      case Element::IMON_BATT:
        return get_imon_batt( lut );

      case Element::VMON_1V1:
        return get_vmon_1v1( lut );

      case Element::VMON_3V3:
        return get_vmon_3v3( lut );

      case Element::VMON_5V0:
        return get_vmon_5v0( lut );

      case Element::VMON_12V:
        return get_vmon_12v( lut );

      default:
        mbed_assert_continue_msg( false, "Invalid sensor element: %d", static_cast<size_t>( channel ) );
        return 0.0f;
    }
  }

  /*---------------------------------------------------------------------------
  Private Function Implementations
  ---------------------------------------------------------------------------*/

  static float calc_thermistor_temp( const float vOut )
  {
    constexpr float t0      = 25.0f;     /* NTC Nomnal 25C */
    constexpr float r0      = 10'000.0f; /* NTC Nomnal 10k */
    constexpr float r_fixed = 10'000.0f; /* 10k Upper resistor*/
    constexpr float beta    = 3'380.0f;  /* NTC Beta: 445-2550-1-ND */

    return Analog::calculateTempBeta( vOut, 5.0f, beta, r_fixed, r0, t0 );
  }


  static float get_ltc_avg_current( const LookupType lut )
  {
    using namespace HW::LTC7871;

    constexpr float IMON_RNG_MIN = 0.4f;
    constexpr float IMON_RNG_MAX = 2.5f;
    constexpr float IMON_ZERO    = 1.25f;

    static float s_cached_avg_current = 0.0f;
    if( lut == LookupType::CACHED )
    {
      return s_cached_avg_current;
    }

    /*-------------------------------------------------------------------------
    Get operational parameters of the LTC7871
    -------------------------------------------------------------------------*/
    float k = 0;
    switch( getILim() )
    {
      case ILim::V0:
      case ILim::V1_4:
        k = 40.0f;
        break;

      case ILim::V3_4:
      case ILim::V5:
        k = 20.0f;
        break;

      default:
        mbed_assert_continue_msg( false, "Invalid ILim value: %d", static_cast<size_t>( getILim() ) );
        return 0.0f;
    }

    // TODO: Pull this info from the board spec.
    float RSense = getRSense();
    mbed_dbg_assert_msg( RSense > 0.0f, "Invalid RSense value: %.2f", RSense );

    /*-------------------------------------------------------------------------
    Calculate the current based on the IMON voltage (pg. 16)
    -------------------------------------------------------------------------*/
    float Vimon = HW::ADC::getVoltage( HW::ADC::Channel::LTC_IMON );
    if( ( Vimon < IMON_RNG_MIN ) || ( Vimon > IMON_RNG_MAX ) )
    {
      mbed_assert_continue_msg( !HW::LTC7871::available(), "IMON voltage out of range: %.2f", Vimon );
      Vimon = std::min( IMON_RNG_MAX, std::max( IMON_RNG_MIN, Vimon ) );
    }

    s_cached_avg_current = ( 6.0f * ( Vimon - IMON_ZERO ) ) / ( k * RSense );
    return s_cached_avg_current;
  }


  static float get_high_side_voltage( const LookupType lut )
  {
    // TODO: Pull this info from the board spec.
    constexpr float R1 = 470'000.0f;
    constexpr float R2 = 15'000.0f;

    static float s_cached_value = 0.0f;
    if( lut == LookupType::CACHED )
    {
      return s_cached_value;
    }

    float Vout = HW::ADC::getVoltage( HW::ADC::Channel::HV_DC_SENSE );
    s_cached_value = Analog::calculateVoltageDividerInput( Vout, R1, R2 );
    return s_cached_value;
  }


  static float get_low_side_voltage( const LookupType lut )
  {
    // TODO: Pull this info from the board spec.
    constexpr float R1 = 470'000.0f;
    constexpr float R2 = 27'000.0f;

    static float s_cached_value = 0.0f;
    if( lut == LookupType::CACHED )
    {
      return s_cached_value;
    }

    float Vout = HW::ADC::getVoltage( HW::ADC::Channel::LV_DC_SENSE );
    s_cached_value = Analog::calculateVoltageDividerInput( Vout, R1, R2 );
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
    float temp    = HW::ADC::getVoltage( HW::ADC::Channel::RP2040_TEMP );
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

    float vOut = HW::ADC::getVoltage( HW::ADC::Channel::TEMP_SENSE_0 );
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

    float vOut = HW::ADC::getVoltage( HW::ADC::Channel::TEMP_SENSE_1 );
    s_cached_value = calc_thermistor_temp( vOut );
    return s_cached_value;
  }


  static float get_imon_batt( const LookupType lut )
  {
    static constexpr float IMON_R_SENSE = 0.0002f; // 0.2mOhm

    if( BSP::getBoardRevision() < 2 )
    {
      return 0.0f;
    }

    static float s_cached_value = 0.0f;
    if( lut == LookupType::CACHED )
    {
      return s_cached_value;
    }

    /*-------------------------------------------------------------------------
    Reverse the voltage divider + op-amp gain to get the current. The INA180A4
    has a gain of 200V/V.
    -------------------------------------------------------------------------*/
    float vOut = HW::ADC::getVoltage( HW::ADC::Channel::IMON_BATT );
    float vMsr = Analog::calculateVoltageDividerInput( vOut, 5'100.0f, 10'000.0f );

    s_cached_value = ( vMsr / IMON_R_SENSE ) * ( 1.0f / 200.0f );
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

    float vOut = HW::ADC::getVoltage( HW::ADC::Channel::VMON_3V3 );
    s_cached_value = Analog::calculateVoltageDividerInput( vOut, 10'000.0f, 10'000.0f );
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

    float vOut = HW::ADC::getVoltage( HW::ADC::Channel::VMON_5V0 );
    s_cached_value = Analog::calculateVoltageDividerInput( vOut, 10'000.0f, 10'000.0f );
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

    float vOut = HW::ADC::getVoltage( HW::ADC::Channel::VMON_12V );
    s_cached_value = Analog::calculateVoltageDividerInput( vOut, 100'000.0f, 10'000.0f );
    return s_cached_value;
  }
}  // namespace Data
