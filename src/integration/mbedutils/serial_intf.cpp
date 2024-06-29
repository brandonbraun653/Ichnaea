/******************************************************************************
 *  File Name:
 *    uart_intf.cpp
 *
 *  Description:
 *    Ichnaea integration of the mb UART interface
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/interfaces/serial_intf.hpp>
#include "src/hw/uart.hpp"

namespace mb::intf::serial
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  bool lock( const size_t channel, const size_t timeout )
  {
    return false;
  }


  void unlock( const size_t channel )
  {
  }


  int async_write( const size_t channel, const void *data, const size_t length )
  {
    return -1;
  }


  void on_tx_complete( const size_t channel, TXCompleteCallback callback )
  {
  }


  void abort_write( const size_t channel )
  {
  }


  int async_read( const size_t channel, void *data, const size_t length )
  {
    return -1;
  }


  void on_rx_complete( const size_t channel, RXCompleteCallback callback )
  {
  }


  void abort_read( const size_t channel )
  {
  }

}    // namespace mb::intf::uart
