/******************************************************************************
 *  File Name:
 *    control_thread.cpp
 *
 *  Description:
 *    Control thread to implement MPPT and other system control behaviors
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "src/bsp/board_map.hpp"
#include "src/hw/fan.hpp"
#include "src/hw/uart.hpp"
#include "src/threads/ichnaea_threads.hpp"
#include "src/system/system_sensor.hpp"
#include <mbedutils/logging.hpp>
#include <mbedutils/rpc.hpp>

namespace Threads
{
  /* Serial driver memory */
  static mb::hw::serial::SerialDriver              s_serial_driver;
  static etl::bip_buffer_spsc_atomic<uint8_t, 512> txBuffer;
  static etl::bip_buffer_spsc_atomic<uint8_t, 512> rxBuffer;

  /* RPC Server memory */
  static mb::rpc::server::Server            s_rpc_server;
  static mb::rpc::server::ServiceStorage<5> s_service_registry;
  static mb::rpc::StreamStorage<128>        s_rpc_rx_buffer;
  static mb::rpc::StreamStorage<128>        s_rpc_tx_buffer;
  static mb::rpc::ScratchStorage<64>        s_rpc_tx_scratch;
  static mb::rpc::ScratchStorage<64>        s_rpc_rx_scratch;

  /* Ping Service Memory */
  static mb::rpc::services::PingStorage     s_ping_storage;
  static mb::rpc::services::PingService     s_ping_service;


  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void controlThread( void )
  {
    using namespace mb::rpc;
    using namespace mb::hw::serial;

    /*-------------------------------------------------------------------------
    Configure the serial driver for the RPC server
    -------------------------------------------------------------------------*/
    Config serial_cfg;
    serial_cfg.channel  = HW::UART::UART_BMS;
    serial_cfg.rxBuffer = &rxBuffer;
    serial_cfg.txBuffer = &txBuffer;

    s_serial_driver.open( serial_cfg );

    /*-------------------------------------------------------------------------
    Configure the RPC server
    -------------------------------------------------------------------------*/
    server::Config rpc_cfg;
    rpc_cfg.iostream      = &s_serial_driver;
    rpc_cfg.rxBuffer      = &s_rpc_rx_buffer;
    rpc_cfg.svcReg        = &s_service_registry;
    rpc_cfg.txScratch     = etl::span<uint8_t>{ s_rpc_tx_scratch.data(), s_rpc_tx_scratch.size() };
    rpc_cfg.rxScratch     = etl::span<uint8_t>{ s_rpc_rx_scratch.data(), s_rpc_rx_scratch.size() };

    s_rpc_server.open( rpc_cfg );

    /*-------------------------------------------------------------------------
    Configure the Ping Service
    -------------------------------------------------------------------------*/

    // TODO: Might be able to constexpr bind this data with a compile time function?

    s_ping_service.name            = "Ping";
    s_ping_service.svcId           = mbed_rpc_BuiltinService_SVC_PING;
    s_ping_service.reqId           = mbed_rpc_BuiltinMessage_MSG_PING;
    s_ping_service.reqData         = &s_ping_storage.req;
    s_ping_service.reqDecodeBuffer = etl::span<uint8_t>{ s_ping_storage.reqDecodeBuffer.data(), s_ping_storage.reqDecodeBuffer.size() };
    s_ping_service.rspId           = mbed_rpc_BuiltinMessage_MSG_PING;
    s_ping_service.rspData         = &s_ping_storage.rsp;
    s_ping_service.rspEncodeBuffer = etl::span<uint8_t>{ s_ping_storage.rspEncodeBuffer.data(), s_ping_storage.rspEncodeBuffer.size() };

    /*-------------------------------------------------------------------------
    Bind builtin services and messages to the server
    -------------------------------------------------------------------------*/
    mbed_assert_continue( s_rpc_server.addService( &s_ping_service ) );
    sleep_ms( 100 );

    while( 1 )
    {
      s_rpc_server.runServices();
      sleep_ms( 25 );
    }
  }
}    // namespace Threads
