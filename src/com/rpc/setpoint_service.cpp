/******************************************************************************
 *  File Name:
 *    setpoint_service.cpp
 *
 *  Description:
 *    Implements the setpoint RPC service
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "mbed_rpc.pb.h"
#include <src/app/app_power.hpp>
#include <src/com/rpc/rpc_services.hpp>
#include <src/system/system_util.hpp>
#include <src/version.hpp>

namespace COM::RPC
{
  /*---------------------------------------------------------------------------
  Service Implementations
  ---------------------------------------------------------------------------*/

  mb::rpc::ErrId SetpointService::processRequest()
  {
    /*-------------------------------------------------------------------------
    Ignore the message if it's not for us
    -------------------------------------------------------------------------*/
    if( request.node_id != System::identity() )
    {
      return mbed_rpc_ErrorCode_ERR_SVC_NO_RSP;
    }

    /*-------------------------------------------------------------------------
    Validate input data type
    -------------------------------------------------------------------------*/
    if( request.which_value_oneof != ichnaea_SetpointRequest_float_type_tag )
    {
      mbed_assert_continue_msg( false, "Expected float type for setpoint. Got %d.", request.which_value_oneof );
      return mbed_rpc_ErrorCode_ERR_SVC_FAILED;
    }

    /*-------------------------------------------------------------------------
    Process the request
    -------------------------------------------------------------------------*/
    auto result          = mbed_rpc_ErrorCode_ERR_NO_ERROR;
    response.status      = ichnaea_SetpointError_ERR_SETPOINT_NO_ERROR;
    response.has_message = false;

    switch( request.field )
    {
      case ichnaea_SetpointField_SETPOINT_OUTPUT_VOLTAGE:
        if( !App::Power::setOutputVoltage( request.value_oneof.float_type ) )
        {
          response.status = ichnaea_SetpointError_ERR_SETPOINT_INVALID;
        }
        break;

      case ichnaea_SetpointField_SETPOINT_OUTPUT_CURRENT:
        if( !App::Power::setOutputCurrentLimit( request.value_oneof.float_type ) )
        {
          response.status = ichnaea_SetpointError_ERR_SETPOINT_INVALID;
        }
        break;

      default:
        mbed_assert_continue_msg( false, "Unknown setpoint field: %d", request.field );
        result          = mbed_rpc_ErrorCode_ERR_SVC_FAILED;
        response.status = ichnaea_SetpointError_ERR_SETPOINT_NOT_SUPPORTED;
        break;
    }

    return result;
  }

}    // namespace COM::RPC
