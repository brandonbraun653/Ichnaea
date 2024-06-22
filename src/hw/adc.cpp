/******************************************************************************
 *  File Name:
 *    adc.cpp
 *
 *  Description:
 *    Ichnaea ADC driver implementation
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "etl/array.h"
#include "src/bsp/board_map.hpp"
#include "src/hw/adc.hpp"
#include "src/system/system_error.hpp"
#include "src/system/system_util.hpp"

namespace HW::ADC
{
  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static etl::array<float, Channel::NUM_OPTIONS>  s_cached_voltage;
  static etl::array<size_t, Channel::NUM_OPTIONS> s_adc_ch_sel;
  static recursive_mutex_t                        s_adc_mutex;
  static uint                                     s_adc_select;

  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Maps one of the GPIO pins to the corresponding ADC input.
   * Used in adc_select_input().
   *
   * @param pin   GPIO pin to map
   * @return uint Corresponding ADC input
   */
  static uint map_adc_input( const uint pin )
  {
    switch( pin )
    {
      case 26:
        return 0;
      case 27:
        return 1;
      case 28:
        return 2;
      case 29:
        return 3;
      default:
        Panic::throwError( Panic::ErrorCode::INVALID_PARAM );
        return 255;
    }
  }

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    auto config = BSP::getIOConfig();

    /*-------------------------------------------------------------------------
    Initialize the ADC peripheral. The voltages being measured in the system
    aren't high frequency or high precision, so the default settings should
    be sufficient.
    -------------------------------------------------------------------------*/
    adc_init();

    /*-------------------------------------------------------------------------
    Initialize the GPIO for ADC selection and input
    -------------------------------------------------------------------------*/
    adc_gpio_init( config.adc.sensePin );

    gpio_init( config.gpio.adcSel0 );
    gpio_set_dir( config.gpio.adcSel0, GPIO_OUT );
    gpio_put( config.gpio.adcSel0, 0 );

    gpio_init( config.gpio.adcSel1 );
    gpio_set_dir( config.gpio.adcSel1, GPIO_OUT );
    gpio_put( config.gpio.adcSel1, 0 );

    gpio_init( config.gpio.adcSel2 );
    gpio_set_dir( config.gpio.adcSel2, GPIO_OUT );
    gpio_put( config.gpio.adcSel2, 0 );

    /*-------------------------------------------------------------------------
    Initialize static memory
    -------------------------------------------------------------------------*/
    s_adc_select = map_adc_input( config.adc.sensePin );
    s_adc_ch_sel.fill( 0 );
    s_cached_voltage.fill( -1.0f );
    recursive_mutex_init( &s_adc_mutex );

    /*-------------------------------------------------------------------------
    Map a channel to the correct ADC input channel select bits
    -------------------------------------------------------------------------*/
    s_adc_ch_sel[ Channel::LTC_IMON ]     = 0;
    s_adc_ch_sel[ Channel::TEMP_SENSE_0 ] = 1;
    s_adc_ch_sel[ Channel::TEMP_SENSE_1 ] = 2;
    s_adc_ch_sel[ Channel::HV_DC_SENSE ]  = 4;
    s_adc_ch_sel[ Channel::LV_DC_SENSE ]  = 5;
  }


  void postSequence()
  {
    /*-------------------------------------------------------------------------
    Make sure we get something for each channel. Can't really determine true
    accuracy here, but we can at least verify that the ADC is working.
    -------------------------------------------------------------------------*/
    for( size_t i = 0; i < Channel::NUM_OPTIONS; i++ )
    {
      volatile float voltage = getVoltage( i );
      if( voltage < 0.0f )
      {
        Panic::throwError( Panic::ErrorCode::POST_FAIL );
      }
    }
  }


  float getVoltage( const size_t channel )
  {
    constexpr float ADC_VOLT_PER_BIT = 3.3f / 4096.0f;

    /*-------------------------------------------------------------------------
    Input Protection
    -------------------------------------------------------------------------*/
    if( channel >= Channel::NUM_OPTIONS )
    {
      Panic::throwError( Panic::ErrorCode::INVALID_PARAM );
      return -1.0f;
    }

    if( System::inISR() )
    {
      /* Trying to control mutexes, HW, and timing in an ISR? No no.*/
      Panic::throwError( Panic::ErrorCode::INVALID_CONTEXT );
      return -1.0f;
    }

    /*-------------------------------------------------------------------------
    Perform the ADC conversion
    -------------------------------------------------------------------------*/
    float result = -1.0f;
    recursive_mutex_enter_blocking( &s_adc_mutex );
    {
      if( channel == Channel::RP2040_TEMP )
      {
        /*---------------------------------------------------------------------
        The RP2040 has a built in temperature sensor that can be read via the
        ADC. This is a special case and doesn't require the 74HC4051 mux.
        ---------------------------------------------------------------------*/
        adc_select_input( 4 );
        result = static_cast<float>( adc_read() ) * ADC_VOLT_PER_BIT;
        recursive_mutex_exit( &s_adc_mutex );
        return result;
      }
      /* Otherwise we're sampling through the mux */

      /*-----------------------------------------------------------------------
      Select the correct ADC channel
      -----------------------------------------------------------------------*/
      adc_select_input( s_adc_select );
      gpio_put( BSP::getIOConfig().gpio.adcSel0, static_cast<bool>( s_adc_ch_sel[ channel ] & 0x01 ) );
      gpio_put( BSP::getIOConfig().gpio.adcSel1, static_cast<bool>( s_adc_ch_sel[ channel ] & 0x02 ) );
      gpio_put( BSP::getIOConfig().gpio.adcSel2, static_cast<bool>( s_adc_ch_sel[ channel ] & 0x04 ) );

      /*-----------------------------------------------------------------------
      Insert >35nS delay to account for 74HC4051 switching times. See table
      5.7 for more information. The below calculation isn't perfectly cycle
      accurate, but it does guarantee a minimum delay, which is the goal.
      -----------------------------------------------------------------------*/
      constexpr size_t delay_time_ns = 50;
      constexpr size_t max_core_freq = 133000000;
      constexpr float  clk_period_ns = ( 1.0f / max_core_freq ) * 1e9;
      constexpr size_t delay_cycles  = delay_time_ns / clk_period_ns;

      #if defined( ICHNAEA_EMBEDDED )
      for( size_t i = 0; i < delay_cycles; i++ )
      {
        __asm volatile( "nop" );
      }
      #endif /* ICHNAEA_EMBEDDED */

      /*-----------------------------------------------------------------------
      Perform the ADC conversion
      -----------------------------------------------------------------------*/
      result = static_cast<float>( adc_read() ) * ADC_VOLT_PER_BIT;
      s_cached_voltage[ channel ] = result;
    }
    recursive_mutex_exit( &s_adc_mutex );

    return result;
  }


  float getCachedVoltage( const size_t channel )
  {
    if( channel >= Channel::NUM_OPTIONS )
    {
      Panic::throwError( Panic::ErrorCode::INVALID_PARAM );
      return -1.0f;
    }

    return s_cached_voltage[ channel ];
  }
}  // namespace HW::ADC
