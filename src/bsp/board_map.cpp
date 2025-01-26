/******************************************************************************
 *  File Name:
 *    board_map.cpp
 *
 *  Description:
 *    Helper methods for selecting the appropriate board IO map
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <src/bsp/board_map.hpp>
#include <src/bsp/board_v1.hpp>
#include <src/bsp/board_v2.hpp>
#include <src/system/system_error.hpp>
#include <src/system/panic_handlers.hpp>
#include <cstring>
#include <cstddef>
#include <mbedutils/drivers/hardware/utility.hpp>

#if defined( ICHNAEA_SIMULATOR )
#include <src/sim/sim_ports.hpp>
#endif

namespace BSP
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  /**
   * @brief IO pin used to read the board version from the ADC
   *
   * Not all versions of the hardware will have this pin available, mostly due
   * to not thinking about it when the hardware was designed. This will be used
   * to determine the board version at runtime and subsequently select the
   * correct IO map.
   */
  static constexpr size_t PIN_ADC_BOARD_VERSION     = 28;
  static constexpr size_t ADC_CHANNEL_BOARD_VERSION = 2;

  static constexpr size_t ADC_BIT_SIZE     = 12u;
  static constexpr float  ADC_REF_VOLTAGE  = 3.3f;
  static constexpr float  ADC_MAX_VALUE    = static_cast<float>( ( 1u << ADC_BIT_SIZE ) - 1u );
  static constexpr float  ADC_VOLT_PER_BIT = ADC_REF_VOLTAGE / ADC_MAX_VALUE;

  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static IOConfig s_board_map;    // Final resolved board IO mapping

  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/

  template<typename T>
  constexpr bool isInArray( T value, const T *array, size_t size )
  {
    for( size_t i = 0; i < size; ++i )
    {
      if( array[ i ] == value )
      {
        return true;
      }
    }
    return false;
  }


  /**
   * @brief Converts the board revision voltage to a version number
   *
   * @param voltage Voltage measured from the board revision pin
   * @return int Version number of the board
   */
  static int convert_analog_board_rev_to_version( const float voltage )
  {
    if( 0.25f <= voltage && voltage < 0.35f )
    {
      return 2;
    }
    else
    {
      // Fill this in later when we have more versions on V2.
      Panic::throwError( Panic::ErrorCode::ERR_BOARD_VERSION_READ_FAIL );
    }

    return -1;
  }


  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void powerUp()
  {
    /*-------------------------------------------------------------------------
    Route error handling behavior through the panic system
    -------------------------------------------------------------------------*/
    auto cb = Panic::ErrorCallback::create<Panic::Handlers::FailToReadBoardVersion>();
    Panic::registerHandler( Panic::ErrorCode::ERR_BOARD_VERSION_READ_FAIL, cb );

/*-------------------------------------------------------------------------
Initialize and read the ADC pin used for board versioning
-------------------------------------------------------------------------*/
#if defined( ICHNAEA_EMBEDDED )
    adc_init();
    adc_gpio_init( PIN_ADC_BOARD_VERSION );
    gpio_set_pulls( PIN_ADC_BOARD_VERSION, false, false );
    adc_select_input( ADC_CHANNEL_BOARD_VERSION );
    const float version_voltage = static_cast<float>( adc_read() ) * ADC_VOLT_PER_BIT;

    int selected_version = convert_analog_board_rev_to_version( version_voltage );
    if( selected_version < 0 )
    {
      Panic::throwError( Panic::ErrorCode::ERR_BOARD_VERSION_READ_FAIL );
    }
#else
    int selected_version = 2;
#endif

    /*-------------------------------------------------------------------------
    Select the correct IO map based on the board version
    -------------------------------------------------------------------------*/
    s_board_map = {};

    if( selected_version == 1 )
    {
      s_board_map.majorVersion                     = BSPV1::BOARD_VERSION_MAJOR;
      s_board_map.minorVersion                     = BSPV1::BOARD_VERSION_MINOR;
      s_board_map.adc[ ADC_MUTLIPLEXED_SENSE ].pin = BSPV1::ADC::PIN_SENSE;
      s_board_map.gpio[ GPIO_LTC_ADCSEL0 ].pin     = BSPV1::GPIO::PIN_OUT_ADC_SEL_0;
      s_board_map.gpio[ GPIO_LTC_ADCSEL1 ].pin     = BSPV1::GPIO::PIN_OUT_ADC_SEL_1;
      s_board_map.gpio[ GPIO_LTC_ADCSEL2 ].pin     = BSPV1::GPIO::PIN_OUT_ADC_SEL_2;
      s_board_map.gpio[ GPIO_LTC_CCM ].pin         = BSPV1::GPIO::PIN_OUT_LTC_CCM;
      s_board_map.gpio[ GPIO_LTC_DCM ].pin         = BSPV1::GPIO::PIN_OUT_LTC_DCM;
      s_board_map.gpio[ GPIO_LTC_RUN ].pin         = BSPV1::GPIO::PIN_IO_LTC_RUN;
      s_board_map.gpio[ GPIO_SPI_CS_LTC ].pin      = BSPV1::SPI::PIN_CS_0;
      s_board_map.pwm[ PWM_FAN_CONTROL ].pin       = BSPV1::PWM::PIN_OUT_FAN_CTL;
      s_board_map.pwm[ PWM_FAN_SENSE ].pin         = BSPV1::PWM::PIN_IN_FAN_SENSE;
      s_board_map.pwm[ PWM_LED_STATUS_0 ].pin      = BSPV1::GPIO::PIN_OUT_LED_STATUS_0;
      s_board_map.pwm[ PWM_LED_STATUS_1 ].pin      = BSPV1::GPIO::PIN_OUT_LED_STATUS_1;
      s_board_map.pwm[ PWM_LED_STATUS_2 ].pin      = BSPV1::GPIO::PIN_OUT_LED_STATUS_2;
      s_board_map.pwm[ PWM_LED_STATUS_3 ].pin      = BSPV1::GPIO::PIN_OUT_LED_STATUS_3;
      s_board_map.pwm[ PWM_LTC_SYNC ].pin          = BSPV1::PWM::PIN_OUT_LTC_SYNC;
      s_board_map.spi[ SPI_LTC7871 ].miso          = BSPV1::SPI::PIN_MISO;
      s_board_map.spi[ SPI_LTC7871 ].mosi          = BSPV1::SPI::PIN_MOSI;
      s_board_map.spi[ SPI_LTC7871 ].sck           = BSPV1::SPI::PIN_SCK;
      s_board_map.spi[ SPI_LTC7871 ].pHw           = spi1;
      s_board_map.uart[ UART_DEBUG ].rx            = BSPV1::UART::PIN_RX;
      s_board_map.uart[ UART_DEBUG ].tx            = BSPV1::UART::PIN_TX;
      s_board_map.uart[ UART_DEBUG ].pHw           = uart0;
    }
    else if( selected_version == 2 )
    {
      s_board_map.majorVersion                     = BSPV2::BOARD_VERSION_MAJOR;
      s_board_map.minorVersion                     = BSPV2::BOARD_VERSION_MINOR;
      s_board_map.vmon_3v3_vdiv_r1                 = 10'000.0f;
      s_board_map.vmon_3v3_vdiv_r2                 = 10'000.0f;
      s_board_map.vmon_5v0_vdiv_r1                 = 10'000.0f;
      s_board_map.vmon_5v0_vdiv_r2                 = 10'000.0f;
      s_board_map.vmon_12v_vdiv_r1                 = 100'000.0f;
      s_board_map.vmon_12v_vdiv_r2                 = 10'000.0f;
      s_board_map.vmon_load_vdiv_r1                = 470'000.0f;
      s_board_map.vmon_load_vdiv_r2                = 27'000.0f;
      s_board_map.imon_load_vdiv_r1                = 5'100.0f;
      s_board_map.imon_load_vdiv_r2                = 10'000.0f;
      s_board_map.imon_load_rsense                 = 0.0002f;
      s_board_map.imon_load_opamp_gain             = 200.0f;
      s_board_map.vmon_solar_vdiv_r1               = 470'000.0f;
      s_board_map.vmon_solar_vdiv_r2               = 15'000.0f;
      s_board_map.tmon_vdiv_input                  = 5.0f;
      s_board_map.tmon_vdiv_r1_fixed               = 10'000.0f;
      s_board_map.tmon_vdiv_r2_thermistor          = 10'000.0f;
      s_board_map.tmon_beta_25C                    = 3'380.0f; /* Digikey: 445-2550-1-ND */
      s_board_map.ltc_inductor_dcr                 = 0.0016f;
      s_board_map.ltc_vlow_ra                      = 15'000.0f;
      s_board_map.ltc_vlow_rb                      = 470'000.0f;
      s_board_map.ltc_num_phases                   = 6;
      s_board_map.ltc_setcur_rfb                   = 22'000.0f;
      s_board_map.adc[ ADC_MUTLIPLEXED_SENSE ].pin = BSPV2::ADC::PIN_MULTIPLEX;
      s_board_map.adc[ ADC_BOARD_REV ].pin         = BSPV2::ADC::PIN_BOARD_REV;
      s_board_map.adc[ ADC_IMON_BATT ].pin         = BSPV2::ADC::PIN_IMON_BATT;
      s_board_map.adc[ ADC_IMON_FLTR ].pin         = BSPV2::ADC::PIN_IMON_FLTR;
      s_board_map.gpio[ GPIO_LTC_ADCSEL0 ].pin     = BSPV2::GPIO::PIN_OUT_ADC_SEL_0;
      s_board_map.gpio[ GPIO_LTC_ADCSEL1 ].pin     = BSPV2::GPIO::PIN_OUT_ADC_SEL_1;
      s_board_map.gpio[ GPIO_LTC_ADCSEL2 ].pin     = BSPV2::GPIO::PIN_OUT_ADC_SEL_2;
      s_board_map.gpio[ GPIO_LTC_CCM ].pin         = BSPV2::GPIO::PIN_OUT_LTC_CCM;
      s_board_map.gpio[ GPIO_LTC_DCM ].pin         = BSPV2::GPIO::PIN_OUT_LTC_DCM;
      s_board_map.gpio[ GPIO_LTC_RUN ].pin         = BSPV2::GPIO::PIN_OUT_LTC_RUN;
      s_board_map.gpio[ GPIO_LTC_PWMEN ].pin       = BSPV2::GPIO::PIN_OUT_LTC_PWMEN;
      s_board_map.gpio[ GPIO_SPI_CS_LTC ].pin      = BSPV2::SPI::PIN_LTC_CS_0;
      s_board_map.gpio[ GPIO_SPI_CS_NOR ].pin      = BSPV2::SPI::PIN_NOR_CS_0;
      s_board_map.pwm[ PWM_FAN_CONTROL ].pin       = BSPV2::PWM::PIN_OUT_FAN_CTL;
      s_board_map.pwm[ PWM_FAN_SENSE ].pin         = BSPV2::PWM::PIN_IN_FAN_SENSE;
      s_board_map.pwm[ PWM_LED_STATUS_0 ].pin      = BSPV2::PWM::PIN_OUT_LED_STATUS_0;
      s_board_map.pwm[ PWM_LED_STATUS_1 ].pin      = BSPV2::PWM::PIN_OUT_LED_STATUS_1;
      s_board_map.pwm[ PWM_LED_STATUS_2 ].pin      = BSPV2::PWM::PIN_OUT_LED_STATUS_2;
      s_board_map.pwm[ PWM_LED_STATUS_3 ].pin      = BSPV2::PWM::PIN_OUT_LED_STATUS_3;
      s_board_map.pwm[ PWM_LTC_SYNC ].pin          = BSPV2::PWM::PIN_OUT_LTC_SYNC;
      s_board_map.spi[ SPI_NOR_FLASH ].miso        = BSPV2::SPI::PIN_NOR_MISO;
      s_board_map.spi[ SPI_NOR_FLASH ].mosi        = BSPV2::SPI::PIN_NOR_MOSI;
      s_board_map.spi[ SPI_NOR_FLASH ].sck         = BSPV2::SPI::PIN_NOR_SCK;
      s_board_map.spi[ SPI_NOR_FLASH ].pHw         = spi0;
      s_board_map.spi[ SPI_NOR_FLASH ].port        = 0;
      s_board_map.spi[ SPI_LTC7871 ].miso          = BSPV2::SPI::PIN_LTC_MISO;
      s_board_map.spi[ SPI_LTC7871 ].mosi          = BSPV2::SPI::PIN_LTC_MOSI;
      s_board_map.spi[ SPI_LTC7871 ].sck           = BSPV2::SPI::PIN_LTC_SCK;
      s_board_map.spi[ SPI_LTC7871 ].pHw           = spi1;
      s_board_map.spi[ SPI_LTC7871 ].port          = 1;
      s_board_map.uart[ UART_DEBUG ].rx            = BSPV2::UART::PIN_DEBUG_RX;
      s_board_map.uart[ UART_DEBUG ].tx            = BSPV2::UART::PIN_DEBUG_TX;
      s_board_map.uart[ UART_DEBUG ].pHw           = uart0;
      s_board_map.uart[ UART_BMS ].rx              = BSPV2::UART::PIN_BMS_RX;
      s_board_map.uart[ UART_BMS ].tx              = BSPV2::UART::PIN_BMS_TX;
      s_board_map.uart[ UART_BMS ].pHw             = uart1;

#if defined( ICHNAEA_SIMULATOR )
      s_board_map.uart[ UART_DEBUG ].zmq_endpoint = SIM::kZMQ_EP_PREFIX + SIM::kUART_DEBUG_PORT;
      s_board_map.uart[ UART_BMS ].zmq_endpoint   = SIM::kZMQ_EP_PREFIX + SIM::kUART_BMS_PORT;
#endif    // ICHNAEA_SIMULATOR
    }
    else
    {
      Panic::throwError( Panic::ErrorCode::ERR_BOARD_VERSION_READ_FAIL );
    }
  }


  const IOConfig &getIOConfig()
  {
    return s_board_map;
  }


  unsigned int getPin( const mb::hw::Peripheral type, const size_t port )
  {
    switch( type )
    {
      case mb::hw::Peripheral::PERIPH_ADC:
        mbed_assert( port < s_board_map.adc.size() );
        return s_board_map.adc[ port ].pin;

      case mb::hw::Peripheral::PERIPH_GPIO:
        mbed_assert( port < s_board_map.gpio.size() );
        return s_board_map.gpio[ port ].pin;

      case mb::hw::Peripheral::PERIPH_PWM:
        mbed_assert( port < s_board_map.pwm.size() );
        return s_board_map.pwm[ port ].pin;

      default:
        mbed_assert_always();
        return 0;
    }
  }

  void *getHardware( const mb::hw::Peripheral type, const size_t port )
  {
    switch( type )
    {
      case mb::hw::Peripheral::PERIPH_SPI:
        mbed_assert( port < s_board_map.spi.size() );
        return s_board_map.spi[ port ].pHw;

      case mb::hw::Peripheral::PERIPH_UART:
        mbed_assert( port < s_board_map.uart.size() );
        return s_board_map.uart[ port ].pHw;

      default:
        mbed_assert_always();
        return nullptr;
    }
  }


  uint8_t getBoardRevision()
  {
    return s_board_map.majorVersion;
  }

#if defined( ICHNAEA_SIMULATOR )
  std::string getZMQEndpoint( const mb::hw::Peripheral type, const size_t port )
  {
    switch( type )
    {
      case mb::hw::Peripheral::PERIPH_UART:
        mbed_assert( port < s_board_map.uart.size() );
        return s_board_map.uart[ port ].zmq_endpoint;

      default:
        break;
    }

    throw std::runtime_error( "Invalid peripheral type" );
  }
#endif /* ICHNAEA_SIMULATOR */

}    // namespace BSP
