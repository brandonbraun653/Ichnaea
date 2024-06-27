/******************************************************************************
 *  File Name:
 *    system_logging.cpp
 *
 *  Description:
 *    Ichnaea system logging implementation details
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "src/system/system_logging.hpp"
#include <mbedutils/logging.hpp>
#include <mbedutils/assert.hpp>

namespace Logging
{
  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static mbedutils::logging::UARTSink        s_uart_sink;
  static mbedutils::logging::SinkHandle_rPtr s_uart_handle;


  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    using namespace mbedutils::logging;

    mbedutils::logging::initialize();

    s_uart_sink.assignChannel( 0 );
    s_uart_sink.logLevel = Level::LVL_TRACE;
    s_uart_sink.enabled  = true;

    s_uart_handle = SinkHandle_rPtr( &s_uart_sink );
    registerSink( s_uart_handle );

    mbed_assert( ErrCode::ERR_OK == setRootSink( s_uart_handle ), "Failed to set root sink" );
  }

}    // namespace Logging
