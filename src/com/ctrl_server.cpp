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

  /* RPC Server Memory*/
  static mb::rpc::server::Server                     s_rpc_server;
  static mb::rpc::server::Storage<15, 128, 256, 128> s_rpc_server_storage;
  static mb::rpc::message::DescriptorStorage<25>     s_rpc_msg_registry;

  /* Service Declarations */
  static COM::RPC::PingNodeService       s_ping_node_service;
  static COM::RPC::IdentityService       s_identity_service;
  static COM::RPC::ManagerService        s_manager_service;
  static COM::RPC::SetpointService       s_setpoint_service;
  static COM::RPC::SensorService         s_sensor_service;
  static COM::RPC::LTCRegisterSetService s_ltc_register_set_service;
  static COM::RPC::LTCRegisterGetService s_ltc_register_get_service;

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

    /* Ping Node Service */
    mbed_assert_continue( s_rpc_server.addService( &s_ping_node_service ) );
    mbed_assert_continue( mb::rpc::message::addDescriptor( COM::RPC::PingNodeRequest ) );
    mbed_assert_continue( mb::rpc::message::addDescriptor( COM::RPC::PingNodeResponse ) );

    /* Identity Service */
    mbed_assert_continue( s_rpc_server.addService( &s_identity_service ) );
    mbed_assert_continue( mb::rpc::message::addDescriptor( COM::RPC::GetIdRequest ) );
    mbed_assert_continue( mb::rpc::message::addDescriptor( COM::RPC::GetIdResponse ) );

    /* Manager Service */
    mbed_assert_continue( s_rpc_server.addService( &s_manager_service ) );
    mbed_assert_continue( mb::rpc::message::addDescriptor( COM::RPC::ManagerRequest ) );
    mbed_assert_continue( mb::rpc::message::addDescriptor( COM::RPC::ManagerResponse ) );

    /* Setpoint Service */
    mbed_assert_continue( s_rpc_server.addService( &s_setpoint_service ) );
    mbed_assert_continue( mb::rpc::message::addDescriptor( COM::RPC::SetpointRequest ) );
    mbed_assert_continue( mb::rpc::message::addDescriptor( COM::RPC::SetpointResponse ) );

    /* Sensor Service */
    mbed_assert_continue( s_rpc_server.addService( &s_sensor_service ) );
    mbed_assert_continue( mb::rpc::message::addDescriptor( COM::RPC::SensorRequest ) );
    mbed_assert_continue( mb::rpc::message::addDescriptor( COM::RPC::SensorResponse ) );

    /* LTC Register Set Service */
    mbed_assert_continue( s_rpc_server.addService( &s_ltc_register_set_service ) );
    mbed_assert_continue( mb::rpc::message::addDescriptor( COM::RPC::LTCRegisterSetRequest ) );
    mbed_assert_continue( mb::rpc::message::addDescriptor( COM::RPC::LTCRegisterSetResponse ) );

    /* LTC Register Get Service */
    mbed_assert_continue( s_rpc_server.addService( &s_ltc_register_get_service ) );
    mbed_assert_continue( mb::rpc::message::addDescriptor( COM::RPC::LTCRegisterGetRequest ) );
    mbed_assert_continue( mb::rpc::message::addDescriptor( COM::RPC::LTCRegisterGetResponse ) );
  }


  mb::rpc::server::Server &getRPCServer()
  {
    return s_rpc_server;
  }

}    // namespace Control
