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
    auto result = mbed_rpc_ErrorCode_ERR_NO_ERROR;

    switch( request.field )
    {
      case ichnaea_SetpointField_SETPOINT_OUTPUT_VOLTAGE:
        result = set_voltage_output( request.value_oneof.float_type );
        break;

      case ichnaea_SetpointField_SETPOINT_MAX_AVG_OUTPUT_CURRENT:
        result = set_max_current_limit( request.value_oneof.float_type );
        break;

      default:
        mbed_assert_continue_msg( false, "Unknown setpoint field: %d", request.field );
        result = mbed_rpc_ErrorCode_ERR_SVC_FAILED;
        break;
    }

    return result;
  }

  mb::rpc::ErrId SetpointService::set_voltage_output( const float voltage )
  {
    ( void )voltage;
    return mbed_rpc_ErrorCode_ERR_SVC_FAILED;
  }


  mb::rpc::ErrId SetpointService::set_max_current_limit( const float current )
  {
    ( void )current;
    return mbed_rpc_ErrorCode_ERR_SVC_FAILED;
  }

}  // namespace COM::RPC
