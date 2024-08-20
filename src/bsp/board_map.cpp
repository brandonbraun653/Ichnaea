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
#include "src/bsp/board_map.hpp"
#include "src/bsp/board_v1.hpp"
#include "src/system/system_error.hpp"
#include "src/system/panic_handlers.hpp"
#include <cstring>
#include <mbedutils/drivers/hardware/utility.hpp>

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
  static constexpr size_t PIN_ADC_BOARD_VERSION     = 29;
  static constexpr size_t ADC_CHANNEL_BOARD_VERSION = 4;

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
  constexpr bool isInArray(T value, const T* array, size_t size)
  {
    for (size_t i = 0; i < size; ++i)
    {
      if (array[i] == value)
      {
        return true;
      }
    }
    return false;
  }


  static int convert_adc_to_version( const float voltage )
  {
    if( 0.0 <= voltage && voltage < 1.0 )
    {
      return 1;
    }
    else
    {
      // Fill this in later when we have more versions on V2.
      Panic::throwError( Panic::ErrorCode::ERR_BOARD_VERSION_READ_FAIL );
    }

    return -1;
  }

  /*---------------------------------------------------------------------------
  Assertions
  ---------------------------------------------------------------------------*/

  static_assert( isInArray( BSPV1::GPIO::PIN_OUT_LED_STATUS_0, Internal::UNMAPPED_LED_PINS, Internal::UNMAPPED_LED_NUM ) );
  static_assert( isInArray( BSPV1::GPIO::PIN_OUT_LED_STATUS_1, Internal::UNMAPPED_LED_PINS, Internal::UNMAPPED_LED_NUM ) );
  static_assert( isInArray( BSPV1::GPIO::PIN_OUT_LED_STATUS_2, Internal::UNMAPPED_LED_PINS, Internal::UNMAPPED_LED_NUM ) );
  static_assert( isInArray( BSPV1::GPIO::PIN_OUT_LED_STATUS_3, Internal::UNMAPPED_LED_PINS, Internal::UNMAPPED_LED_NUM ) );

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void powerUp()
  {
    /*-------------------------------------------------------------------------
    Route error handling behavior through the panic system
    -------------------------------------------------------------------------*/
    Panic::registerHandler( Panic::ErrorCode::ERR_BOARD_VERSION_READ_FAIL, Panic::Handlers::FailToReadBoardVersion );

    /*-------------------------------------------------------------------------
    Initialize and read the ADC pin used for board versioning
    -------------------------------------------------------------------------*/
    adc_init();
    adc_gpio_init( PIN_ADC_BOARD_VERSION );
    gpio_set_pulls( PIN_ADC_BOARD_VERSION, false, true );
    adc_select_input( ADC_CHANNEL_BOARD_VERSION );
    const float version_voltage = static_cast<float>( adc_read() ) * ADC_VOLT_PER_BIT;

    int selected_version = convert_adc_to_version( version_voltage );
    if ( selected_version < 0 )
    {
      Panic::throwError( Panic::ErrorCode::ERR_BOARD_VERSION_READ_FAIL );
    }

    s_board_map.majorVersion = BSPV1::BOARD_VERSION_MAJOR;
    s_board_map.minorVersion = BSPV1::BOARD_VERSION_MINOR;

    /*-------------------------------------------------------------------------
    Select the correct IO map based on the board version
    -------------------------------------------------------------------------*/
    memset( &s_board_map, 0, sizeof( IOConfig ) );

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
      s_board_map.gpio[ GPIO_SPI_CS0 ].pin         = BSPV1::SPI::PIN_CS_0;
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
  }


  const IOConfig &getIOConfig()
  {
    return s_board_map;
  }


  uint getPin( const mb::hw::Peripheral type, const size_t port )
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
}    // namespace BSP
