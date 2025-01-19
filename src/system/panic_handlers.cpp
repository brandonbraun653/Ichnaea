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
#include <src/bsp/board_map.hpp>
#include <src/system/panic_handlers.hpp>
#include <src/system/system_control.hpp>

namespace Panic::Handlers
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  bool DefaultHandler( const Panic::ErrorCode &code )
  {
    ( void )code;

    // TODO: Publish the information somewhere? LED, UART, etc...
    System::reset( System::ResetType::REBOOT );
    return false;
  }


  bool NoopHandler( const Panic::ErrorCode &code )
  {
    ( void )code;
    return true;
  }


  bool FailToReadBoardVersion( const Panic::ErrorCode &code )
  {
    ( void )code;

    /*-------------------------------------------------------------------------
    Reset the board to prevent further operation. If there truly is a problem
    with the board versioning, it's not safe to continue.
    -------------------------------------------------------------------------*/
    System::reset( System::ResetType::REBOOT );
    return false;
  }
}    // namespace Panic::Handlers
