/******************************************************************************
 *  File Name:
 *    manager_service.cpp
 *
 *  Description:
 *    Implements the manager RPC service
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/

#include "src/com/rpc/rpc_services.hpp"
#include "src/hw/ltc7871.hpp"
#include "src/system/system_util.hpp"
#include "src/version.hpp"
#include <mbedutils/util.hpp>

namespace COM::RPC
{
  /*---------------------------------------------------------------------------
  Service Implementations
  ---------------------------------------------------------------------------*/

  mb::rpc::ErrId ManagerService::processRequest()
  {
    mb::rpc::ErrId rpc_result = mbed_rpc_ErrorCode_ERR_NO_ERROR;

    /*-------------------------------------------------------------------------
    Default initialize the response
    -------------------------------------------------------------------------*/
    response.status      = ichnaea_ManagerError_ERR_CMD_NO_ERROR;
    response.has_message = false;
    clear_array( response.message );

    /*-------------------------------------------------------------------------
    Process the incoming request
    -------------------------------------------------------------------------*/
    switch( request.command )
    {
      case ichnaea_ManagerCommand_CMD_ENGAGE_OUTPUT:
        rpc_result = engage_output();
        break;

      case ichnaea_ManagerCommand_CMD_DISENGAGE_OUTPUT:
        rpc_result = disengage_output();
        break;

      default:
        response.status = ichnaea_ManagerError_ERR_CMD_INVALID;
        break;
    }

    return rpc_result;
  }


  mb::rpc::ErrId ManagerService::engage_output()
  {
    /*-------------------------------------------------------------------------
    Ignore requests that are not intended for this node
    -------------------------------------------------------------------------*/
    if( request.node_id != System::identity() )
    {
      return mbed_rpc_ErrorCode_ERR_SVC_NO_RSP;
    }

    response.has_message = false;
    response.status      = ichnaea_ManagerError_ERR_CMD_NO_ERROR;

    /*-------------------------------------------------------------------------
    Engage the output if not already enabled
    -------------------------------------------------------------------------*/
    HW::LTC7871::powerOn();

    if( auto mode = HW::LTC7871::getMode(); mode != HW::LTC7871::DriverMode::NORMAL_OPERATION )
    {
      response.status      = ichnaea_ManagerError_ERR_CMD_FAILED;
      response.has_message = true;
      snprintf( response.message, sizeof( response.message ), "Unexpected mode: %d", mode );
    }

    return mbed_rpc_ErrorCode_ERR_NO_ERROR;
  }


  mb::rpc::ErrId ManagerService::disengage_output()
  {
    /*-------------------------------------------------------------------------
    Ignore requests that are not intended for this node
    -------------------------------------------------------------------------*/
    if( request.node_id != System::identity() )
    {
      return mbed_rpc_ErrorCode_ERR_SVC_NO_RSP;
    }

    response.has_message = false;
    response.status      = ichnaea_ManagerError_ERR_CMD_NO_ERROR;

    /*-------------------------------------------------------------------------
    Disengage the output
    -------------------------------------------------------------------------*/
    HW::LTC7871::powerOff();

    if( auto mode = HW::LTC7871::getMode(); mode != HW::LTC7871::DriverMode::DISABLED )
    {
      response.status      = ichnaea_ManagerError_ERR_CMD_FAILED;
      response.has_message = true;
      snprintf( response.message, sizeof( response.message ), "Unexpected mode: %d", mode );
    }

    return mbed_rpc_ErrorCode_ERR_NO_ERROR;
  }

}  // namespace COM::RPC
