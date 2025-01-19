/******************************************************************************
 *  File Name:
 *    sim_services.cpp
 *
 *  Description:
 *    Constructs the simulation control server gRPC services
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <grpcpp/server_builder.h>
#include <memory>
#include <src/sim/generated/sim_intf.grpc.pb.h>
#include <src/sim/generated/sim_intf.pb.h>
#include <src/sim/sim_ports.hpp>
#include <src/sim/sim_service_environment_spoofer.hpp>
#include <src/sim/sim_services.hpp>
#include <string>
#include <thread>

namespace SIM
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  static const std::string kEnvironmentSpooferAddress = "0.0.0.0:" + SIM::kEnvironmentSpooferPort;

  /*---------------------------------------------------------------------------
  Private Data
  ---------------------------------------------------------------------------*/

  static std::unique_ptr<std::thread> spooferThread;
  static std::unique_ptr<grpc::Server> spooferServer;

  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/

  static void runEnvironmentSpoofer()
  {
    SIM::Service::EnvironmentSpooferImpl service;
    grpc::ServerBuilder builder;
    builder.AddListeningPort( kEnvironmentSpooferAddress, grpc::InsecureServerCredentials() );
    builder.RegisterService( &service );
    spooferServer = builder.BuildAndStart();
    std::cout << "EnvironmentSpoofer service listening on " << kEnvironmentSpooferAddress << std::endl;
    spooferServer->Wait();
  }

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void spawnServices()
  {
    /*-------------------------------------------------------------------------
    Spawn all services into their own worker threads
    -------------------------------------------------------------------------*/
    spooferThread = std::make_unique<std::thread>( runEnvironmentSpoofer );
  }


  void destroyServices()
  {
    /*-------------------------------------------------------------------------
    Shutdown the spoofer service
    -------------------------------------------------------------------------*/
    if( spooferThread )
    {
      spooferServer->Shutdown();
      spooferThread->join();
      spooferThread.reset();
    }
  }

}  // namespace SIM
