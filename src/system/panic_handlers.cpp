/******************************************************************************
 *  File Name:
 *    panic_handlers.cpp
 *
 *  Description:
 *    Implementation of common panic handlers
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "src/bsp/board_map.hpp"
#include "src/system/panic_handlers.hpp"
#include "src/system/system_control.hpp"

namespace Panic::Handlers
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  PANIC_HANDLER_DEF( DefaultHandler )
  {
    // TODO: Publish the information somewhere? LED, UART, etc...
    System::reset( System::ResetType::REBOOT );
    return false;
  }


  PANIC_HANDLER_DEF( NoopHandler )
  {
    return true;
  }


  PANIC_HANDLER_DEF( FailToReadBoardVersion )
  {
    using namespace BSP::Internal;

    /*-------------------------------------------------------------------------
    Init the expected board LEDs. This is constant for all versions.
    -------------------------------------------------------------------------*/
    constexpr uint32_t LED_DELAY_HI = 250;
    constexpr uint32_t LED_DELAY_LO = 100;

    for( size_t i = 0; i < UNMAPPED_LED_NUM; i++ )
    {
      gpio_init( UNMAPPED_LED_PINS[ i ] );
      gpio_set_dir( UNMAPPED_LED_PINS[ i ], GPIO_OUT );
      gpio_put( UNMAPPED_LED_PINS[ i ], 1 );
    }

    /*-------------------------------------------------------------------------
    Flash the LEDs in a pattern to indicate the error.
    -------------------------------------------------------------------------*/
    for( size_t i = 0; i < 3; i++ )
    {
      for( size_t j = 0; j < UNMAPPED_LED_NUM; j++ )
      {
        gpio_put( UNMAPPED_LED_PINS[ j ], 0 );
      }

      sleep_ms( LED_DELAY_HI );

      for( size_t j = 0; j < UNMAPPED_LED_NUM; j++ )
      {
        gpio_put( UNMAPPED_LED_PINS[ j ], 1 );
      }

      sleep_ms( LED_DELAY_LO );
    }

    /*-------------------------------------------------------------------------
    Reset the board to prevent further operation. If there truly is a problem
    with the board versioning, it's not safe to continue.
    -------------------------------------------------------------------------*/
    System::reset( System::ResetType::REBOOT );
    return false;
  }
}    // namespace Panic::Handlers
