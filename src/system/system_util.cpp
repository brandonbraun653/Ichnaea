/******************************************************************************
 *  File Name:
 *    system_util.cpp
 *
 *  Description:
 *    Ichnaea system utility functions and helpers
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "src/bsp/board_map.hpp"
#include "src/system/system_util.hpp"

namespace System
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  bool inISR()
  {
    #if defined( ICHNAEA_EMBEDDED )
    uint32_t result;

    __asm volatile ("MRS %0, ipsr" : "=r" (result) );
    return(result != 0);
    #else
    return false;
    #endif  /* ICHNAEA_EMBEDDED */
  }
}  // namespace System
