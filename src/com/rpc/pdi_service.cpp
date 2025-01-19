/******************************************************************************
 *  File Name:
 *    pdi_service.cpp
 *
 *  Description:
 *    Implements the PDI RPC service
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/logging.hpp>
#include <mbedutils/rpc.hpp>
#include <src/app/app_pdi.hpp>
#include <src/com/rpc/rpc_services.hpp>
#include <src/system/system_db.hpp>
#include <src/system/system_util.hpp>

namespace COM::RPC
{
  /*---------------------------------------------------------------------------
  Service Implementations
  ---------------------------------------------------------------------------*/

  mb::rpc::ErrId PDIReadService::processRequest()
  {
    /*-------------------------------------------------------------------------
    Validate the request
    -------------------------------------------------------------------------*/
    if( request.node_id != System::identity() )
    {
      return mbed_rpc_ErrorCode_ERR_SVC_NO_RSP;
    }

    /*-------------------------------------------------------------------------
    Find the PDI data and encode it into the response buffer
    -------------------------------------------------------------------------*/
    auto key  = static_cast<App::PDI::PDIKey>( request.pdi_id );
    auto size = System::Database::pdiDB().encode( key, response.data.bytes, sizeof( response.data.bytes ) );

    if( size <= 0 )
    {
      LOG_WARN( "Failed to read PDI key %d", key );
      response.success = false;
      return mb::rpc::ErrId::mbed_rpc_ErrorCode_ERR_SVC_FAILED;
    }

    response.success   = true;
    response.data.size = size;
    return mb::rpc::ErrId::mbed_rpc_ErrorCode_ERR_NO_ERROR;
  }


  mb::rpc::ErrId PDIWriteService::processRequest()
  {
    /*-------------------------------------------------------------------------
    Validate the request
    -------------------------------------------------------------------------*/
    if( request.node_id != System::identity() )
    {
      return mbed_rpc_ErrorCode_ERR_SVC_NO_RSP;
    }

    /*-------------------------------------------------------------------------
    Process the request
    -------------------------------------------------------------------------*/
    auto key  = static_cast<App::PDI::PDIKey>( request.pdi_id );
    auto size = System::Database::pdiDB().decode( key, request.data.bytes, request.data.size );

    if( size <= 0 )
    {
      LOG_WARN( "Failed to write PDI key %d", key );
      response.success = false;
      return mb::rpc::ErrId::mbed_rpc_ErrorCode_ERR_SVC_FAILED;
    }

    response.success = true;
    return mb::rpc::ErrId::mbed_rpc_ErrorCode_ERR_NO_ERROR;
  }

}    // namespace COM::RPC
