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

#include <mbedutils/rpc.hpp>
#include <mbedutils/threading.hpp>
#include <mbedutils/util.hpp>
#include <src/app/app_power.hpp>
#include <src/app/proto/ichnaea_rpc.pb.h>
#include <src/com/rpc/rpc_services.hpp>
#include <src/hw/ltc7871.hpp>
#include <src/system/system_db.hpp>
#include <src/system/system_sensor.hpp>
#include <src/system/system_shutdown.hpp>
#include <src/system/system_util.hpp>
#include <src/threads/ichnaea_threads.hpp>
#include <src/version.hpp>

namespace COM::RPC
{
  /*---------------------------------------------------------------------------
  Service Implementations
  ---------------------------------------------------------------------------*/

  mb::rpc::ErrId ManagerService::processRequest()
  {
    mb::rpc::ErrId rpc_result = mbed_rpc_ErrorCode_ERR_NO_ERROR;

    /*-------------------------------------------------------------------------
    Ensure the request is intended for this node
    -------------------------------------------------------------------------*/
    if( request.node_id != System::identity() )
    {
      return mbed_rpc_ErrorCode_ERR_SVC_NO_RSP;
    }

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
      case ichnaea_ManagerCommand_CMD_REBOOT:
        rpc_result = reboot();
        break;

      case ichnaea_ManagerCommand_CMD_ENGAGE_OUTPUT:
        rpc_result = engage_output();
        break;

      case ichnaea_ManagerCommand_CMD_DISENGAGE_OUTPUT:
        rpc_result = disengage_output();
        break;

      case ichnaea_ManagerCommand_CMD_FLUSH_PDI_CACHE:
        Threads::sendSignal( Threads::SystemTask::TSK_BACKGROUND_ID, Threads::TSK_MSG_FLUSH_PDI );
        break;

      case ichnaea_ManagerCommand_CMD_ZERO_OUTPUT_CURRENT:
        System::Sensor::Calibration::calibrateImonNoLoadOffset();
        break;

      default:
        response.status = ichnaea_ManagerError_ERR_CMD_INVALID;
        break;
    }

    return rpc_result;
  }


  mb::rpc::ErrId ManagerService::reboot()
  {
    System::Shutdown::initiate();

    response.has_message = false;
    response.status      = ichnaea_ManagerError_ERR_CMD_NO_ERROR;

    return mbed_rpc_ErrorCode_ERR_NO_ERROR;
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
    Attempt to engage the output
    -------------------------------------------------------------------------*/
    if( !App::Power::engageOutput() )
    {
      response.status      = ichnaea_ManagerError_ERR_CMD_FAILED;
      response.has_message = false;
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
    App::Power::disengageOutput();

    return mbed_rpc_ErrorCode_ERR_NO_ERROR;
  }

}    // namespace COM::RPC
