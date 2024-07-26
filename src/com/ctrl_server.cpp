/******************************************************************************
 *  File Name:
 *    ctrl_server.cpp
 *
 *  Description:
 *    Command and control server implementation
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "src/hw/uart.hpp"
#include <mbedutils/drivers/rpc/builtin_services.hpp>
#include <mbedutils/drivers/rpc/rpc_common.hpp>
#include <mbedutils/drivers/rpc/rpc_server.hpp>

namespace Control
{
  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static mb::rpc::server::Server                s_rpc_server;
  static mb::rpc::server::ServiceStorage<5>     s_service_registry;
  static mb::rpc::message::DescriptorStorage<5> s_message_registry;
  static mb::rpc::StreamStorage<128>            s_rpc_rx_buffer;
  static mb::rpc::StreamStorage<1024>           s_rpc_tx_buffer;
  static mb::rpc::ScratchStorage<256>           s_rpc_tx_scratch;
  static mb::rpc::ScratchStorage<128>           s_rpc_rx_scratch;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    using namespace mb::rpc;

    /*-------------------------------------------------------------------------
    Configure the RPC server
    -------------------------------------------------------------------------*/
    message::initialize( &s_message_registry );

    server::Config rpc_cfg;
    rpc_cfg.iostream      = &HW::UART::getDriver( HW::UART::Channel::UART_BMS );
    rpc_cfg.rxBuffer      = &s_rpc_rx_buffer;
    rpc_cfg.svcReg        = &s_service_registry;
    rpc_cfg.txScratch     = etl::span<uint8_t>{ s_rpc_tx_scratch.data(), s_rpc_tx_scratch.size() };
    rpc_cfg.rxScratch     = etl::span<uint8_t>{ s_rpc_rx_scratch.data(), s_rpc_rx_scratch.size() };

    s_rpc_server.open( rpc_cfg );
  }

  mb::rpc::server::Server &getRPCServer()
  {
    return s_rpc_server;
  }

}  // namespace Control
