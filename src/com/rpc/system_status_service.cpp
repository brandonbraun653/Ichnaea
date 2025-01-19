/******************************************************************************
 *  File Name:
 *    system_status_service.cpp
 *
 *  Description:
 *    Implement the system status service
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "mbedutils/interfaces/time_intf.hpp"
#include <src/hw/ltc7871.hpp>
#include <src/app/proto/ichnaea_rpc.pb.h>
#include <src/com/rpc/rpc_services.hpp>

namespace COM::RPC
{
  /*---------------------------------------------------------------------------
  Service Implementations
  ---------------------------------------------------------------------------*/
  mb::rpc::ErrId SystemStatusService::processRequest()
  {
    /*-------------------------------------------------------------------------
    Ignore requests that are not intended for this node
    -------------------------------------------------------------------------*/
    if( request.node_id != System::identity() )
    {
      return mbed_rpc_ErrorCode_ERR_SVC_NO_RSP;
    }

    /*-------------------------------------------------------------------------
    Populate the response
    -------------------------------------------------------------------------*/
    response.timestamp = mb::time::millis();

    switch( HW::LTC7871::getMode() )
    {
      case HW::LTC7871::DriverMode::DISABLED:
        response.output_state = ichnaea_EngageState_DISENGAGED;
        break;

      case HW::LTC7871::DriverMode::ENABLED:
        response.output_state = ichnaea_EngageState_ENGAGED;
        break;

      case HW::LTC7871::DriverMode::FAULTED:
        response.output_state = ichnaea_EngageState_FAULTED;
        break;
    }

    return mbed_rpc_ErrorCode_ERR_NO_ERROR;
  }
}  // namespace COM::RPC
