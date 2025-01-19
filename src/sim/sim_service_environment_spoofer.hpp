/******************************************************************************
 *  File Name:
 *    sim_service_environment_spoofer.hpp
 *
 *  Description:
 *    Environment spoofer service for the simulation control server
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_SIM_SERVICE_ENVIRONMENT_SPOOFER_HPP
#define ICHNAEA_SIM_SERVICE_ENVIRONMENT_SPOOFER_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <src/sim/generated/sim_intf.grpc.pb.h>
#include <src/sim/generated/sim_intf.pb.h>

namespace SIM::Service
{
  /*---------------------------------------------------------------------------
  Environment Spoofer Service
  ---------------------------------------------------------------------------*/

  /**
   * @brief Controls the simulated environment through data spoofing.
   *
   * This is the primary control interface for virtualizing the hardware
   * environment, used by the python testing framework to construct complex
   * testing scenarios.
   */
  class EnvironmentSpooferImpl final : public sim::EnvironmentSpoofer::Service
  {
  public:
    /*-------------------------------------------------------------------------
    Constructor & Destructor
    -------------------------------------------------------------------------*/
    EnvironmentSpooferImpl();
    ~EnvironmentSpooferImpl() = default;

    /*-------------------------------------------------------------------------
    Public Methods
    -------------------------------------------------------------------------*/
    ::grpc::Status SetSolarOCV( ::grpc::ServerContext *context, const ::sim::FloatingPointValue *request, ::google::protobuf::Empty *response ) override;
    ::grpc::Status ApplyResistiveLoad( ::grpc::ServerContext *context, const ::sim::FloatingPointValue *request, ::google::protobuf::Empty *response ) override;
    ::grpc::Status SetBoard12VRail(::grpc::ServerContext* context, const ::sim::FloatingPointValue* request, ::google::protobuf::Empty* response) override;
    ::grpc::Status SetBoard5VRail(::grpc::ServerContext* context, const ::sim::FloatingPointValue* request, ::google::protobuf::Empty* response) override;
    ::grpc::Status SetBoard3V3Rail(::grpc::ServerContext* context, const ::sim::FloatingPointValue* request, ::google::protobuf::Empty* response) override;
    ::grpc::Status SetBoard1V1Rail(::grpc::ServerContext* context, const ::sim::FloatingPointValue* request, ::google::protobuf::Empty* response) override;
    ::grpc::Status SetBoardTemperature(::grpc::ServerContext* context, const ::sim::FloatingPointValue* request, ::google::protobuf::Empty* response) override;
    ::grpc::Status SetBoardFanSpeed(::grpc::ServerContext* context, const ::sim::FloatingPointValue* request, ::google::protobuf::Empty* response) override;
    ::grpc::Status SetOutputVoltage(::grpc::ServerContext* context, const ::sim::FloatingPointValue* request, ::google::protobuf::Empty* response) override;

  };
}    // namespace SIM::Service

#endif /* !ICHNAEA_SIM_SERVICE_ENVIRONMENT_SPOOFER_HPP */
