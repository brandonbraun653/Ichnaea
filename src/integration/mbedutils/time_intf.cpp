/******************************************************************************
 *  File Name:
 *    time_intf.cpp
 *
 *  Description:
 *    Ichnaea integration of the mbedutils time interface
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/interfaces/time_intf.hpp>
#include "hardware/timer.h"

namespace mbedutils::time
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  size_t millis()
  {
    return static_cast<size_t>( time_us_64() / 1000 );
  }

}  // namespace mbedutils::time
