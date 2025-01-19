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
#include <etl/array.h>
#include <src/bsp/board_map.hpp>
#include <src/hw/adc.hpp>
#include <src/system/system_error.hpp>
#include <src/system/system_sensor.hpp>
#include <src/system/system_util.hpp>
#include <mbedutils/logging.hpp>
#include <mbedutils/osal.hpp>

namespace HW::ADC
{
  /*---------------------------------------------------------------------------
  Structures
  ---------------------------------------------------------------------------*/

  struct ADCConfig
  {
    float cached_voltage; /**< Last measured voltage */
    int   adc_mux_sel;    /**< If multiplexed, the particular pin configuration */
    int   phy_adc_input;  /**< Physical ADC channel (0-3) for measurement */
  };

  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static etl::array<ADCConfig, Channel::NUM_OPTIONS> s_adc_config;
  static mb::osal::mb_recursive_mutex_t              s_adc_mutex;

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
        Panic::throwError( Panic::ErrorCode::ERR_INVALID_PARAM );
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
    adc_set_temp_sensor_enabled( true );

    /*-------------------------------------------------------------------------
    Initialize the GPIO for ADC selection and input
    -------------------------------------------------------------------------*/
    adc_gpio_init( config.adc[ BSP::ADC_MUTLIPLEXED_SENSE ].pin );

    if( BSP::getIOConfig().majorVersion >= 2 )
    {
      adc_gpio_init( config.adc[ BSP::ADC_BOARD_REV ].pin );
      adc_gpio_init( config.adc[ BSP::ADC_IMON_FLTR ].pin );
      adc_gpio_init( config.adc[ BSP::ADC_IMON_BATT ].pin );
    }

    gpio_init( config.gpio[ BSP::GPIO_LTC_ADCSEL0 ].pin );
    gpio_set_dir( config.gpio[ BSP::GPIO_LTC_ADCSEL0 ].pin, GPIO_OUT );
    gpio_put( config.gpio[ BSP::GPIO_LTC_ADCSEL0 ].pin, 0 );

    gpio_init( config.gpio[ BSP::GPIO_LTC_ADCSEL1 ].pin );
    gpio_set_dir( config.gpio[ BSP::GPIO_LTC_ADCSEL1 ].pin, GPIO_OUT );
    gpio_put( config.gpio[ BSP::GPIO_LTC_ADCSEL1 ].pin, 0 );

    gpio_init( config.gpio[ BSP::GPIO_LTC_ADCSEL2 ].pin );
    gpio_set_dir( config.gpio[ BSP::GPIO_LTC_ADCSEL2 ].pin, GPIO_OUT );
    gpio_put( config.gpio[ BSP::GPIO_LTC_ADCSEL2 ].pin, 0 );

    /*-------------------------------------------------------------------------
    Initialize static memory
    -------------------------------------------------------------------------*/
    s_adc_config.fill( { 0.0f, -1, -1 } );
    mbed_assert( mb::osal::buildRecursiveMutexStrategy( s_adc_mutex ) );

    /*-------------------------------------------------------------------------
    Configure the ADC channels based on the board version
    -------------------------------------------------------------------------*/
    if( BSP::getIOConfig().majorVersion == 1 )
    {
      /*-----------------------------------------------------------------------
      Version 1 uses a multiplexed ADC input for all sensors. The 74HC4051
      analog switch is used to select the correct channel.
      -----------------------------------------------------------------------*/
      s_adc_config[ Channel::LTC_IMON ].phy_adc_input = map_adc_input( config.adc[ BSP::ADC_MUTLIPLEXED_SENSE ].pin );
      s_adc_config[ Channel::LTC_IMON ].adc_mux_sel   = 0;

      s_adc_config[ Channel::TEMP_SENSE_0 ].phy_adc_input = map_adc_input( config.adc[ BSP::ADC_MUTLIPLEXED_SENSE ].pin );
      s_adc_config[ Channel::TEMP_SENSE_0 ].adc_mux_sel   = 1;

      s_adc_config[ Channel::TEMP_SENSE_1 ].phy_adc_input = map_adc_input( config.adc[ BSP::ADC_MUTLIPLEXED_SENSE ].pin );
      s_adc_config[ Channel::TEMP_SENSE_1 ].adc_mux_sel   = 2;

      s_adc_config[ Channel::HV_DC_SENSE ].phy_adc_input = map_adc_input( config.adc[ BSP::ADC_MUTLIPLEXED_SENSE ].pin );
      s_adc_config[ Channel::HV_DC_SENSE ].adc_mux_sel   = 4;

      s_adc_config[ Channel::LV_DC_SENSE ].phy_adc_input = map_adc_input( config.adc[ BSP::ADC_MUTLIPLEXED_SENSE ].pin );
      s_adc_config[ Channel::LV_DC_SENSE ].adc_mux_sel   = 5;
    }
    else if( BSP::getIOConfig().majorVersion == 2 )
    {
      /*-----------------------------------------------------------------------
      Configure the independent ADC channels
      -----------------------------------------------------------------------*/
      s_adc_config[ Channel::RP2040_TEMP ].phy_adc_input = 4;
      s_adc_config[ Channel::BOARD_REV ].phy_adc_input   = map_adc_input( config.adc[ BSP::ADC_BOARD_REV ].pin );
      s_adc_config[ Channel::IMON_LOAD ].phy_adc_input   = map_adc_input( config.adc[ BSP::ADC_IMON_BATT ].pin );
      s_adc_config[ Channel::LTC_IMON ].phy_adc_input    = map_adc_input( config.adc[ BSP::ADC_IMON_FLTR ].pin );

      /*-----------------------------------------------------------------------
      Configure the multiplexed ADC channels
      -----------------------------------------------------------------------*/
      s_adc_config[ Channel::VMON_1V1 ].phy_adc_input = map_adc_input( config.adc[ BSP::ADC_MUTLIPLEXED_SENSE ].pin );
      s_adc_config[ Channel::VMON_1V1 ].adc_mux_sel   = 0;

      s_adc_config[ Channel::VMON_3V3 ].phy_adc_input = map_adc_input( config.adc[ BSP::ADC_MUTLIPLEXED_SENSE ].pin );
      s_adc_config[ Channel::VMON_3V3 ].adc_mux_sel   = 3;

      s_adc_config[ Channel::VMON_5V0 ].phy_adc_input = map_adc_input( config.adc[ BSP::ADC_MUTLIPLEXED_SENSE ].pin );
      s_adc_config[ Channel::VMON_5V0 ].adc_mux_sel   = 1;

      s_adc_config[ Channel::VMON_12V ].phy_adc_input = map_adc_input( config.adc[ BSP::ADC_MUTLIPLEXED_SENSE ].pin );
      s_adc_config[ Channel::VMON_12V ].adc_mux_sel   = 5;

      s_adc_config[ Channel::TEMP_SENSE_0 ].phy_adc_input = map_adc_input( config.adc[ BSP::ADC_MUTLIPLEXED_SENSE ].pin );
      s_adc_config[ Channel::TEMP_SENSE_0 ].adc_mux_sel   = 4;

      s_adc_config[ Channel::TEMP_SENSE_1 ].phy_adc_input = map_adc_input( config.adc[ BSP::ADC_MUTLIPLEXED_SENSE ].pin );
      s_adc_config[ Channel::TEMP_SENSE_1 ].adc_mux_sel   = 2;

      s_adc_config[ Channel::HV_DC_SENSE ].phy_adc_input = map_adc_input( config.adc[ BSP::ADC_MUTLIPLEXED_SENSE ].pin );
      s_adc_config[ Channel::HV_DC_SENSE ].adc_mux_sel   = 6;

      s_adc_config[ Channel::LV_DC_SENSE ].phy_adc_input = map_adc_input( config.adc[ BSP::ADC_MUTLIPLEXED_SENSE ].pin );
      s_adc_config[ Channel::LV_DC_SENSE ].adc_mux_sel   = 7;
    }
    else
    {
      Panic::throwError( Panic::ErrorCode::ERR_INVALID_PARAM );
    }
  }


  void postSequence()
  {
    using namespace System::Sensor;

    /*-------------------------------------------------------------------------
    Make sure we get something for each channel. Can't really determine true
    accuracy here, but we can at least verify that the ADC is working.
    -------------------------------------------------------------------------*/
    for( size_t i = 0; i < Channel::NUM_OPTIONS; i++ )
    {
      volatile float voltage = getVoltage( i );
      if( voltage < 0.0f )
      {
        Panic::throwError( Panic::ErrorCode::ERR_POST_FAIL );
      }
    }

    LOG_DEBUG( "RP2040 Temp: %.2fC", getMeasurement( Element::RP2040_TEMP, LookupType::REFRESH ) );
    LOG_DEBUG( "Board Temp 0: %.2fC", getMeasurement( Element::BOARD_TEMP_0, LookupType::REFRESH ) );
    LOG_DEBUG( "Board Temp 1: %.2fC", getMeasurement( Element::BOARD_TEMP_1, LookupType::REFRESH ) );
    LOG_DEBUG( "LTC Current: %.2fA", getMeasurement( Element::IMON_LTC_AVG, LookupType::REFRESH ) );
    LOG_DEBUG( "Charge Current: %.2fA", getMeasurement( Element::IMON_LOAD, LookupType::REFRESH ) );
    LOG_DEBUG( "1.1V Rail: %.2fV", getMeasurement( Element::VMON_1V1, LookupType::REFRESH ) );
    LOG_DEBUG( "3.3V Rail: %.2fV", getMeasurement( Element::VMON_3V3, LookupType::REFRESH ) );
    LOG_DEBUG( "5.0V Rail: %.2fV", getMeasurement( Element::VMON_5V0, LookupType::REFRESH ) );
    LOG_DEBUG( "12V Rail: %.2fV", getMeasurement( Element::VMON_12V, LookupType::REFRESH ) );
    LOG_DEBUG( "Solar Voltage: %.2fV", getMeasurement( Element::VMON_SOLAR_INPUT, LookupType::REFRESH ) );
    LOG_DEBUG( "Batt Voltage: %.2fV", getMeasurement( Element::VMON_LOAD, LookupType::REFRESH ) );
  }


  float getVoltage( const size_t channel )
  {
    /**
     * @brief RP2040 ADC is 12-bit, 3.3V reference
     */
    constexpr float ADC_VOLT_PER_BIT = 3.3f / 4096.0f;

    /*-------------------------------------------------------------------------
    Input Protection
    -------------------------------------------------------------------------*/
    if( channel >= Channel::NUM_OPTIONS )
    {
      Panic::throwError( Panic::ErrorCode::ERR_INVALID_PARAM );
      return -1.0f;
    }

    if( System::inISR() )
    {
      /* Trying to control mutexes, HW, and timing in an ISR? No no.*/
      Panic::throwError( Panic::ErrorCode::ERR_INVALID_CONTEXT );
      return -1.0f;
    }

    mb::thread::RecursiveLockGuard lock( s_adc_mutex );

    /*-------------------------------------------------------------------------
    Perform the ADC conversion
    -------------------------------------------------------------------------*/
    auto &adc_cfg = s_adc_config[ channel ];

    adc_select_input( adc_cfg.phy_adc_input );

    if( adc_cfg.adc_mux_sel >= 0 )
    {
      gpio_put( BSP::getIOConfig().gpio[ BSP::GPIO_LTC_ADCSEL0 ].pin, static_cast<bool>( adc_cfg.adc_mux_sel & 0x01 ) );
      gpio_put( BSP::getIOConfig().gpio[ BSP::GPIO_LTC_ADCSEL1 ].pin, static_cast<bool>( adc_cfg.adc_mux_sel & 0x02 ) );
      gpio_put( BSP::getIOConfig().gpio[ BSP::GPIO_LTC_ADCSEL2 ].pin, static_cast<bool>( adc_cfg.adc_mux_sel & 0x04 ) );

      /*-----------------------------------------------------------------------
      Insert >35nS delay to account for 74HC4051 switching times. See table
      5.7 for more information. The below calculation isn't perfectly cycle
      accurate, but it does guarantee a minimum delay, which is the goal.
      -----------------------------------------------------------------------*/
      constexpr size_t delay_time_ns = 50;
      constexpr size_t max_core_freq = 133000000;
      constexpr float  clk_period_ns = ( 1.0f / max_core_freq ) * 1e9;
      constexpr size_t delay_cycles  = delay_time_ns / clk_period_ns;

      for( size_t i = 0; i < delay_cycles; i++ )
      {
        __asm volatile( "nop" );
      }
    }

    /*-----------------------------------------------------------------------
    Average three samples to reduce noise. None of the signals we're reading
    are high frequency, so this should be fine.
    -----------------------------------------------------------------------*/
    float result = 0.0f;
    for( size_t i = 0; i < 3; i++ )
    {
      result += static_cast<float>( adc_read() );
      busy_wait_us( 50 );
    }

    result /= 3.0f;
    result *= ADC_VOLT_PER_BIT;

    adc_cfg.cached_voltage = result;

    return result;
  }


  float getCachedVoltage( const size_t channel )
  {
    if( channel >= Channel::NUM_OPTIONS )
    {
      Panic::throwError( Panic::ErrorCode::ERR_INVALID_PARAM );
      return -1.0f;
    }

    return s_adc_config[ channel ].cached_voltage;
  }
}    // namespace HW::ADC
