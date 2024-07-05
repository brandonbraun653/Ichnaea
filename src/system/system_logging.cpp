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
#include "src/hw/uart.hpp"
#include "src/system/system_logging.hpp"
#include <etl/string.h>
#include <mbedutils/assert.hpp>
#include <mbedutils/drivers/hardware/serial.hpp>
#include <mbedutils/interfaces/serial_intf.hpp>
#include <mbedutils/logging.hpp>

namespace Logging
{
  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static mb::logging::SerialSink                   s_uart_sink;
  static mb::logging::SinkHandle_rPtr              s_uart_handle;
  static mb::hw::serial::SerialDriver              s_serial_driver;
  static etl::bip_buffer_spsc_atomic<uint8_t, 512> txBuffer;
  static etl::bip_buffer_spsc_atomic<uint8_t, 512> rxBuffer;


  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    using namespace mb::logging;
    using namespace mb::hw::serial;

    /*-------------------------------------------------------------------------
    Initialize the logging framework
    -------------------------------------------------------------------------*/
    mb::logging::initialize();

    /*-------------------------------------------------------------------------
    Configure a SerialSink to use the BMS UART channel
    -------------------------------------------------------------------------*/
    Config serial_cfg;
    serial_cfg.channel  = HW::UART::UART_BMS;
    serial_cfg.rxBuffer = &rxBuffer;
    serial_cfg.txBuffer = &txBuffer;

    s_uart_sink.assignDriver( s_serial_driver, serial_cfg );
    s_uart_sink.logLevel = Level::LVL_TRACE;
    s_uart_sink.enabled  = true;

    /*-------------------------------------------------------------------------
    Register the sink with the logging framework and set it as the root output
    for all log messages.
    -------------------------------------------------------------------------*/
    s_uart_handle = SinkHandle_rPtr( &s_uart_sink );
    mbed_assert( ErrCode::ERR_OK == registerSink( s_uart_handle ) );
    mbed_assert( ErrCode::ERR_OK == setRootSink( s_uart_handle ) );

    LOG_TRACE( "Ichnaea booting up" );
  }

}    // namespace Logging
