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

  static int convert_adc_to_version( const float voltage )
  {
    if( 0.0 <= voltage && voltage < 0.5 )
    {
      return 1;
    }
    else if( 0.5 <= voltage && voltage < 1.0 )
    {
      return 2;
    }
    else if( 1.0 <= voltage && voltage < 1.5 )
    {
      return 3;
    }
    else if( 1.5 <= voltage && voltage < 2.0 )
    {
      return 4;
    }
    else if( 2.0 <= voltage && voltage < 2.5 )
    {
      return 5;
    }
    else if( 2.5 <= voltage && voltage < 3.0 )
    {
      return 6;
    }
    else if( 3.0 <= voltage && voltage < 3.3 )
    {
      return 7;
    }

    return -1;
  }


  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void powerUp()
  {
    memset( &s_board_map, 0, sizeof( IOConfig ) );

    /*-------------------------------------------------------------------------
    Route error handling behavior through the panic system
    -------------------------------------------------------------------------*/
    Panic::registerHandler( Panic::ErrorCode::BOARD_VERSION_READ_FAIL, Panic::Handlers::FailToReadBoardVersion );

    /*-------------------------------------------------------------------------
    Initialize and read the ADC pin used for board versioning
    -------------------------------------------------------------------------*/
    #if defined( ICHNAEA_EMBEDDED )
    adc_init();
    adc_gpio_init( PIN_ADC_BOARD_VERSION );
    adc_select_input( ADC_CHANNEL_BOARD_VERSION );
    const float version_voltage = static_cast<float>( adc_read() ) * ADC_VOLT_PER_BIT;
    #else
    const float version_voltage = 0.0f;
    #endif

    int selected_version = convert_adc_to_version( version_voltage );
    if ( selected_version < 0 )
    {
      Panic::throwSystemError( Panic::ErrorCode::BOARD_VERSION_READ_FAIL );
    }


    s_board_map.majorVersion = BSPV1::BOARD_VERSION_MAJOR;
    s_board_map.minorVersion = BSPV1::BOARD_VERSION_MINOR;

    /*-------------------------------------------------------------------------

    -------------------------------------------------------------------------*/
    // TODO: Perform static assertions on the oops leds to ensure they match
  }


  const IOConfig &getIOConfig()
  {
    return s_board_map;
  }
}    // namespace BSP
