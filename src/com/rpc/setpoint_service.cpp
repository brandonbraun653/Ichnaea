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

#include "src/com/rpc/rpc_services.hpp"
#include "src/system/system_util.hpp"
#include "src/version.hpp"

namespace COM::RPC
{
  /*---------------------------------------------------------------------------
  Service Implementations
  ---------------------------------------------------------------------------*/

  mb::rpc::ErrId SetpointService::processRequest()
  {
    return mbed_rpc_ErrorCode_ERR_NO_ERROR;
  }

}  // namespace COM::RPC
