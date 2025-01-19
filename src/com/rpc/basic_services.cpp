/******************************************************************************
 *  File Name:
 *    basic_services.cpp
 *
 *  Description:
 *    Implements very simple RPC services for the Ichnaea system
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/

#include <src/com/rpc/rpc_services.hpp>
#include <src/system/system_util.hpp>
#include <src/version.hpp>

namespace COM::RPC
{
  /*---------------------------------------------------------------------------
  Service Implementations
  ---------------------------------------------------------------------------*/

  mb::rpc::ErrId PingNodeService::processRequest()
  {
    /*-------------------------------------------------------------------------
    Assuming the node ID is the same as the system ID, we can respond
    immediately. Otherwise, don't send anything back.
    -------------------------------------------------------------------------*/
    if( request.node_id == System::identity() )
    {
      return mbed_rpc_ErrorCode_ERR_NO_ERROR;
    }
    else
    {
      return mbed_rpc_ErrorCode_ERR_SVC_NO_RSP;
    }
  }


  mb::rpc::ErrId IdentityService::processRequest()
  {
    response.unique_id = System::identity();
    response.ver_major = System::kMajorVersion;
    response.ver_minor = System::kMinorVersion;
    response.ver_patch = System::kPatchVersion;

    return mbed_rpc_ErrorCode_ERR_NO_ERROR;
  }

}  // namespace COM::RPC
