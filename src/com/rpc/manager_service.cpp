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

      case ichnaea_ManagerCommand_CMD_DISENGAGE_OUTPUT:
        rpc_result = disengage_output();

      default:
        response.status = ichnaea_ManagerError_ERR_CMD_INVALID;
    }

    return rpc_result;
  }


  mb::rpc::ErrId ManagerService::engage_output()
  {
    //HW::LTC7871::initialize();

    //HW::LTC7871::postSequence();
    return mbed_rpc_ErrorCode_ERR_NO_ERROR;
  }


  mb::rpc::ErrId ManagerService::disengage_output()
  {
    return mbed_rpc_ErrorCode_ERR_NO_ERROR;
  }

}  // namespace COM::RPC
