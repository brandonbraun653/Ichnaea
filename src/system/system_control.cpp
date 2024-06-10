/******************************************************************************
 *  File Name:
 *    system_control.cpp
 *
 *  Description:
 *    Implementation of high level system control behaviors
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "src/bsp/board_map.hpp"
#include "src/system/system_control.hpp"

namespace System
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void reset( const ResetType target )
  {
#if defined( ICHNAEA_EMBEDDED )
    switch( target )
    {
      case ResetType::BOOTLOADER:
        reset_usb_boot( 0, 1 );
        break;

      case ResetType::REBOOT:
      default:
        reset_usb_boot( 0, 0 );
        break;
    }
#else
    ( void )target;
#endif
  }

}    // namespace System
