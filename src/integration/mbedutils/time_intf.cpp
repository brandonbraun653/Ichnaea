/******************************************************************************
 *  File Name:
 *    time_intf.cpp
 *
 *  Description:
 *    Ichnaea integration of the mb time interface
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/interfaces/time_intf.hpp>
#include "pico/time.h"

namespace mb::time
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  size_t millis()
  {
    return to_ms_since_boot( get_absolute_time() );
  }

}  // namespace mb::time
