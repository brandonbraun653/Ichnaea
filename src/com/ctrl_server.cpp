/******************************************************************************
 *  File Name:
 *    ctrl_server.cpp
 *
 *  Description:
 *    Command and control server implementation using an RPC interface
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/memory.hpp>
#include <mbedutils/rpc.hpp>
#include <src/com/async_messages.hpp>
#include <src/com/rpc/rpc_messages.hpp>
#include <src/com/rpc/rpc_services.hpp>
#include <src/hw/uart.hpp>

namespace Control
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  static constexpr size_t RPC_MAX_SERVICES       = 15;
  static constexpr size_t RPC_MAX_MESSAGES       = 50;
  static constexpr size_t RPC_MAX_MSG_SIZE       = 608;    // PDI read/write
  static constexpr size_t RPC_RX_STREAM_BUF_SIZE = 3 * RPC_MAX_MSG_SIZE;
  static constexpr size_t RPC_RX_TRANSCODE_BUF_SIZE =
      ALIGN_UP( COBS_DECODE_DST_BUF_LEN_MAX( RPC_MAX_MSG_SIZE ), sizeof( uint32_t ) );
  static constexpr size_t RPC_TX_TRANSCODE_BUF_SIZE =
      ALIGN_UP( COBS_ENCODE_DST_BUF_LEN_MAX( RPC_MAX_MSG_SIZE ), sizeof( uint32_t ) );

  static_assert( RPC_RX_TRANSCODE_BUF_SIZE % sizeof( uint32_t ) == 0, "Transcode buffer must be word aligned" );
  static_assert( RPC_TX_TRANSCODE_BUF_SIZE % sizeof( uint32_t ) == 0, "Transcode buffer must be word aligned" );


  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  /* RPC Server Memory*/
  static mb::rpc::server::Server                               s_rpc_server;
  static mb::rpc::message::DescriptorStorage<RPC_MAX_MESSAGES> s_rpc_msg_registry;
  static mb::rpc::server::Storage<RPC_MAX_SERVICES, RPC_RX_STREAM_BUF_SIZE, RPC_TX_TRANSCODE_BUF_SIZE,
                                  RPC_RX_TRANSCODE_BUF_SIZE>
      s_rpc_server_storage;

  /* Service Declarations */
  static COM::RPC::PingNodeService              s_ping_node_service;
  static COM::RPC::IdentityService              s_identity_service;
  static COM::RPC::ManagerService               s_manager_service;
  static COM::RPC::SetpointService              s_setpoint_service;
  static COM::RPC::SensorService                s_sensor_service;
  static COM::RPC::PDIReadService               s_pdi_read_service;
  static COM::RPC::PDIWriteService              s_pdi_write_service;
  static COM::RPC::SystemStatusService          s_system_status_service;
  static mb::rpc::service::logger::EraseService s_logger_erase_service;
  static mb::rpc::service::logger::WriteService s_logger_write_service;
  static mb::rpc::service::logger::ReadService  s_logger_read_service;

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
    Add system message definitions
    -------------------------------------------------------------------------*/
    mbed_assert_continue( message::addDescriptor( COM::MSG::Heartbeat ) );

    /*-------------------------------------------------------------------------
    Add the system services
    -------------------------------------------------------------------------*/

    /* Ping Node Service */
    mbed_assert( s_rpc_server.addService( &s_ping_node_service ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::PingNodeRequest ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::PingNodeResponse ) );

    /* Identity Service */
    mbed_assert( s_rpc_server.addService( &s_identity_service ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::GetIdRequest ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::GetIdResponse ) );

    /* Manager Service */
    mbed_assert( s_rpc_server.addService( &s_manager_service ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::ManagerRequest ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::ManagerResponse ) );

    /* Setpoint Service */
    mbed_assert( s_rpc_server.addService( &s_setpoint_service ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::SetpointRequest ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::SetpointResponse ) );

    /* Sensor Service */
    mbed_assert( s_rpc_server.addService( &s_sensor_service ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::SensorRequest ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::SensorResponse ) );

    /* PDI Read Service */
    mbed_assert( s_rpc_server.addService( &s_pdi_read_service ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::PDIReadRequest ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::PDIReadResponse ) );

    /* PDI Write Service */
    mbed_assert( s_rpc_server.addService( &s_pdi_write_service ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::PDIWriteRequest ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::PDIWriteResponse ) );

    /* System Status Service */
    mbed_assert( s_rpc_server.addService( &s_system_status_service ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::SystemStatusRequest ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::SystemStatusResponse ) );

    /* Logger Erase Service */
    mbed_assert( s_rpc_server.addService( &s_logger_erase_service ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::LoggerEraseRequest ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::LoggerEraseResponse ) );

    /* Logger Write Service */
    mbed_assert( s_rpc_server.addService( &s_logger_write_service ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::LoggerWriteRequest ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::LoggerWriteResponse ) );

    /* Logger Read Service */
    mbed_assert( s_rpc_server.addService( &s_logger_read_service ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::LoggerReadRequest ) );
    mbed_assert( mb::rpc::message::addDescriptor( COM::RPC::LoggerReadStreamResponse ) );
  }


  mb::rpc::server::Server &getRPCServer()
  {
    return s_rpc_server;
  }

}    // namespace Control
