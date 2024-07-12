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
#include <mbedutils/drivers/rpc/rpc_server.hpp>
#include <mbedutils/drivers/rpc/builtin_services.hpp>

namespace Control
{
  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static mb::rpc::server::Server s_test_server;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    using namespace mb::rpc;

    server::Config test_config;

    s_test_server.open( test_config );
    s_test_server.addService( services::PingService() );
  }

}  // namespace Control
