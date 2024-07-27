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
#include "src/com/rpc/rpc_services.hpp"
#include "src/com/rpc/rpc_messages.hpp"
#include <mbedutils/drivers/rpc/builtin_services.hpp>
#include <mbedutils/drivers/rpc/rpc_common.hpp>
#include <mbedutils/drivers/rpc/rpc_server.hpp>

namespace Control
{
  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static mb::rpc::server::Server                    s_rpc_server;
  static mb::rpc::server::Storage<5, 128, 256, 128> s_rpc_server_storage;
  static mb::rpc::message::DescriptorStorage<5>     s_rpc_msg_registry;

  static COM::RPC::IdentityService s_identity_service;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    using namespace mb::rpc;

    /*-------------------------------------------------------------------------
    Configure the RPC server
    -------------------------------------------------------------------------*/
    message::initialize( &s_rpc_msg_registry );

    auto rpc_cfg = s_rpc_server_storage.make_config( HW::UART::getDriver( HW::UART::Channel::UART_BMS ) );
    mbed_assert_continue( s_rpc_server.open( rpc_cfg ) );

    /*-------------------------------------------------------------------------
    Add the system services
    -------------------------------------------------------------------------*/

    /* Identity Service */
    mbed_assert_continue( s_rpc_server.addService( &s_identity_service ) );
    mbed_assert_continue( mb::rpc::message::addDescriptor( COM::RPC::GetIdRequest ) );
    mbed_assert_continue( mb::rpc::message::addDescriptor( COM::RPC::GetIdResponse ) );
  }

  mb::rpc::server::Server &getRPCServer()
  {
    return s_rpc_server;
  }

}  // namespace Control
