/******************************************************************************
 *  File Name:
 *    cmn_intf.cpp
 *
 *  Description:
 *    Ichnaea integration of the mb common interface
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/interfaces/cmn_intf.hpp>
#include "src/bsp/board_map.hpp"

namespace mb::hw
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  size_t max_drivers( const Driver driver )
  {
    switch( driver )
    {
      case Driver::UART:
        return BSP::UART_MAX_PORTS;

      default:
        return 0;
    }
  }


  size_t max_driver_index( const Driver driver )
  {
    switch( driver )
    {
      case Driver::UART:
        return BSP::UART_MAX_PORTS - 1;

      default:
        return 0;
    }
  }


  bool is_driver_available( const Driver driver, const size_t channel )
  {
    switch( driver )
    {
      case Driver::UART:
        return channel < BSP::UART_MAX_PORTS;

      default:
        return false;
    }
  }

}  // namespace mb::hw
