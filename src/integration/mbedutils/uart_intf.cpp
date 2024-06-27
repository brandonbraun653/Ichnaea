/******************************************************************************
 *  File Name:
 *    uart_intf.cpp
 *
 *  Description:
 *    Ichnaea integration of the mbedutils UART interface
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/interfaces/uart_intf.hpp>

namespace mbedutils::intf::uart
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  int write( const size_t channel, const uint8_t *data, const size_t length )
  {
    return 0;
  }


  size_t tx_available( const size_t channel )
  {
    return 0;
  }


  int read( const size_t channel, uint8_t *data, const size_t length )
  {
    return 0;
  }


  size_t rx_available( const size_t channel )
  {
    return 0;
  }

}  // namespace mbedutils::intf::uart
