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
#include "src/system/system_sensor.hpp"
#include "src/hw/ltc7871.hpp"
#include "src/hw/adc.hpp"
#include <mbedutils/logging.hpp>
#include <mbedutils/drivers/hardware/analog.hpp>

namespace Sensor
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/


  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/

  static float calculateThermistorTemp( const float vOut )
  {
    constexpr float t0      = 25.0f;     /* NTC Nomnal 25C */
    constexpr float r0      = 10'000.0f; /* NTC Nomnal 10k */
    constexpr float r_fixed = 10'000.0f; /* 10k Upper resistor*/
    constexpr float beta    = 3'380.0f;  /* NTC Beta: 445-2550-1-ND */

    return Analog::calculateTempBeta( vOut, 5.0f, beta, r_fixed, r0, t0 );
  }

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  float getAverageCurrent( const LookupType lut )
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


  float getHighSideVoltage( const LookupType lut )
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


  float getLowSideVoltage( const LookupType lut )
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


  float getRP2040Temp( const LookupType lut )
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


  float getBoardTemp0( const LookupType lut )
  {
    static float s_cached_value = 0.0f;
    if( lut == LookupType::CACHED )
    {
      return s_cached_value;
    }

    float vOut = HW::ADC::getVoltage( HW::ADC::Channel::TEMP_SENSE_0 );
    s_cached_value = calculateThermistorTemp( vOut );
    return s_cached_value;
  }


  float getBoardTemp1( const LookupType lut )
  {
    static float s_cached_value = 0.0f;
    if( lut == LookupType::CACHED )
    {
      return s_cached_value;
    }

    float vOut = HW::ADC::getVoltage( HW::ADC::Channel::TEMP_SENSE_1 );
    s_cached_value = calculateThermistorTemp( vOut );
    return s_cached_value;
  }
}  // namespace Data
