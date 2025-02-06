/******************************************************************************
 *  File Name:
 *    sim_service_environment_spoofer.cpp
 *
 *  Description:
 *    Environment spoofer service for the simulation control server
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "mbedutils/drivers/logging/logging_macros.hpp"
#include "mbedutils/interfaces/time_intf.hpp"
#include "sim_intf.pb.h"
#include <mbedutils/drivers/hardware/analog.hpp>
#include <src/bsp/board_map.hpp>
#include <src/hw/adc.hpp>
#include <src/hw/fan.hpp>
#include <src/hw/ltc7871.hpp>
#include <src/sim/hw/sim_adc.hpp>
#include <src/sim/sim_load.hpp>
#include <src/sim/sim_service_environment_spoofer.hpp>

namespace SIM::Service
{
  /*---------------------------------------------------------------------------
  Environment Spoofer Service
  ---------------------------------------------------------------------------*/

  EnvironmentSpooferImpl::EnvironmentSpooferImpl()
  {
  }


  ::grpc::Status EnvironmentSpooferImpl::ApplyResistiveLoad( ::grpc::ServerContext           *context,
                                                             const ::sim::FloatingPointValue *request,
                                                             ::google::protobuf::Empty       *response )
  {
    ( void )context;
    ( void )response;

    // LOG_TRACE( "Setting resistive load to: %.2f Ohms", request->value() );
    SIM::Load::setResistiveLoad( request->value() );
    return ::grpc::Status::OK;
  }


  ::grpc::Status EnvironmentSpooferImpl::SetSolarOCV( ::grpc::ServerContext *context, const ::sim::FloatingPointValue *request,
                                                      ::google::protobuf::Empty *response )
  {
    ( void )context;
    ( void )response;

    // LOG_TRACE( "Setting solar OCV to: %.2f", request->value() );

    /*-------------------------------------------------------------------------
    Calculate the voltage divider output feeding the ADC
    -------------------------------------------------------------------------*/
    auto  ioConfig = BSP::getIOConfig();
    float adc_reading =
        Analog::calculateVoltageDividerOutput( request->value(), ioConfig.vmon_solar_vdiv_r1, ioConfig.vmon_solar_vdiv_r2 );

    /*-------------------------------------------------------------------------
    Inject the sample into the ADC
    -------------------------------------------------------------------------*/
    HW::ADC::inject_sample( HW::ADC::Channel::HV_DC_SENSE, adc_reading, mb::time::millis() );
    return ::grpc::Status::OK;
  }


  ::grpc::Status EnvironmentSpooferImpl::SetBoard12VRail( ::grpc::ServerContext           *context,
                                                          const ::sim::FloatingPointValue *request,
                                                          ::google::protobuf::Empty       *response )
  {
    ( void )context;
    ( void )response;

    // LOG_TRACE( "Setting 12V rail to: %.2f", request->value() );

    /*-------------------------------------------------------------------------
    Calculate the voltage divider output feeding the ADC
    -------------------------------------------------------------------------*/
    auto  ioConfig = BSP::getIOConfig();
    float adc_reading =
        Analog::calculateVoltageDividerOutput( request->value(), ioConfig.vmon_12v_vdiv_r1, ioConfig.vmon_12v_vdiv_r2 );

    /*-------------------------------------------------------------------------
    Inject the sample into the ADC
    -------------------------------------------------------------------------*/
    HW::ADC::inject_sample( HW::ADC::Channel::VMON_12V, adc_reading, mb::time::millis() );
    return ::grpc::Status::OK;
  }


  ::grpc::Status EnvironmentSpooferImpl::SetBoard5VRail( ::grpc::ServerContext           *context,
                                                         const ::sim::FloatingPointValue *request,
                                                         ::google::protobuf::Empty       *response )
  {
    ( void )context;
    ( void )response;

    // LOG_TRACE( "Setting 5V rail to: %.2f", request->value() );

    /*-------------------------------------------------------------------------
    Calculate the voltage divider output feeding the ADC
    -------------------------------------------------------------------------*/
    auto  ioConfig = BSP::getIOConfig();
    float adc_reading =
        Analog::calculateVoltageDividerOutput( request->value(), ioConfig.vmon_5v0_vdiv_r1, ioConfig.vmon_5v0_vdiv_r2 );

    /*-------------------------------------------------------------------------
    Inject the sample into the ADC
    -------------------------------------------------------------------------*/
    HW::ADC::inject_sample( HW::ADC::Channel::VMON_5V0, adc_reading, mb::time::millis() );
    return ::grpc::Status::OK;
  }


  ::grpc::Status EnvironmentSpooferImpl::SetBoard3V3Rail( ::grpc::ServerContext           *context,
                                                          const ::sim::FloatingPointValue *request,
                                                          ::google::protobuf::Empty       *response )
  {
    ( void )context;
    ( void )response;

    // LOG_TRACE( "Setting 3.3V rail to: %.2f", request->value() );

    /*-------------------------------------------------------------------------
    Calculate the voltage divider output feeding the ADC
    -------------------------------------------------------------------------*/
    auto  ioConfig = BSP::getIOConfig();
    float adc_reading =
        Analog::calculateVoltageDividerOutput( request->value(), ioConfig.vmon_3v3_vdiv_r1, ioConfig.vmon_3v3_vdiv_r2 );

    /*-------------------------------------------------------------------------
    Inject the sample into the ADC
    -------------------------------------------------------------------------*/
    HW::ADC::inject_sample( HW::ADC::Channel::VMON_3V3, adc_reading, mb::time::millis() );
    return ::grpc::Status::OK;
  }


  ::grpc::Status EnvironmentSpooferImpl::SetBoard1V1Rail( ::grpc::ServerContext           *context,
                                                          const ::sim::FloatingPointValue *request,
                                                          ::google::protobuf::Empty       *response )
  {
    ( void )context;
    ( void )response;

    // LOG_TRACE( "Setting 1.1V rail to: %.2f", request->value() );

    /*-------------------------------------------------------------------------
    Inject the sample into the ADC. There is no voltage divider on this rail.
    -------------------------------------------------------------------------*/
    HW::ADC::inject_sample( HW::ADC::Channel::VMON_1V1, request->value(), mb::time::millis() );
    return ::grpc::Status::OK;
  }


  ::grpc::Status EnvironmentSpooferImpl::SetBoardTemperature( ::grpc::ServerContext           *context,
                                                              const ::sim::FloatingPointValue *request,
                                                              ::google::protobuf::Empty       *response )
  {
    ( void )context;
    ( void )response;

    // LOG_TRACE( "Setting board temperature to: %.2fC", request->value() );

    /*-------------------------------------------------------------------------
    Compute the adc reading from the temperature sensor
    -------------------------------------------------------------------------*/
    auto  ioConfig    = BSP::getIOConfig();
    float adc_reading = Analog::calculateVoutFromTemp( request->value(), ioConfig.tmon_vdiv_input, ioConfig.tmon_beta_25C,
                                                       ioConfig.tmon_vdiv_r1_fixed, ioConfig.tmon_vdiv_r2_thermistor, 25.0f );

    HW::ADC::inject_sample( HW::ADC::Channel::TEMP_SENSE_0, adc_reading, mb::time::millis() );
    HW::ADC::inject_sample( HW::ADC::Channel::TEMP_SENSE_1, adc_reading, mb::time::millis() );
    return ::grpc::Status::OK;
  }


  ::grpc::Status EnvironmentSpooferImpl::SetBoardFanSpeed( ::grpc::ServerContext           *context,
                                                           const ::sim::FloatingPointValue *request,
                                                           ::google::protobuf::Empty       *response )
  {
    ( void )context;
    ( void )response;

    // LOG_TRACE( "Setting fan speed to: %.2f RPM", request->value() );

    HW::FAN::setSpeedPercent( request->value() );
    return ::grpc::Status::OK;
  }


  ::grpc::Status EnvironmentSpooferImpl::SetOutputVoltage( ::grpc::ServerContext           *context,
                                                           const ::sim::FloatingPointValue *request,
                                                           ::google::protobuf::Empty       *response )
  {
    ( void )context;
    ( void )response;

    // LOG_TRACE( "Setting output voltage to: %.2f V", request->value() );

    HW::LTC7871::setVoutRef( request->value() );
    return ::grpc::Status::OK;
  }

}    // namespace SIM::Service
