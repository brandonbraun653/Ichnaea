/******************************************************************************
 *  File Name:
 *    rpc_services.hpp
 *
 *  Description:
 *    Ichnaea RPC services
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_RPC_SERVICES_HPP
#define ICHNAEA_RPC_SERVICES_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "src/com/proto/ichnaea.pb.h"
#include <mbedutils/rpc.hpp>

namespace COM::RPC
{
  /*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/

  class PingNodeService : public mb::rpc::BaseService<ichnaea_PingNodeRequest, ichnaea_PingNodeResponse>
  {
  public:
    PingNodeService() :
        BaseService<ichnaea_PingNodeRequest, ichnaea_PingNodeResponse>( "PingNodeService", ichnaea_Service_SVC_PING_NODE,
                                                                        ichnaea_Message_MSG_PING_NODE_REQ,
                                                                        ichnaea_Message_MSG_PING_NODE_RSP ) {};
    ~PingNodeService() = default;

    /**
     * @copydoc IService::processRequest
     */
    mb::rpc::ErrId processRequest() final override;
  };

  class IdentityService : public mb::rpc::BaseService<ichnaea_GetIdRequest, ichnaea_GetIdResponse>
  {
  public:
    IdentityService() :
        BaseService<ichnaea_GetIdRequest, ichnaea_GetIdResponse>( "IdentityService", ichnaea_Service_SVC_IDENTITY,
                                                                  ichnaea_Message_MSG_GET_ID_REQ,
                                                                  ichnaea_Message_MSG_GET_ID_RSP ) {};
    ~IdentityService() = default;

    /**
     * @copydoc IService::processRequest
     */
    mb::rpc::ErrId processRequest() final override;
  };


  class ManagerService : public mb::rpc::BaseService<ichnaea_ManagerRequest, ichnaea_ManagerResponse>
  {
  public:
    ManagerService() :
        BaseService<ichnaea_ManagerRequest, ichnaea_ManagerResponse>( "ManagerService", ichnaea_Service_SVC_MANAGER,
                                                                      ichnaea_Message_MSG_MANAGER_REQ,
                                                                      ichnaea_Message_MSG_MANAGER_RSP ) {};
    ~ManagerService() = default;

    /**
     * @copydoc IService::processRequest
     */
    mb::rpc::ErrId processRequest() final override;

  private:
    mb::rpc::ErrId engage_output();
    mb::rpc::ErrId disengage_output();
  };

  class SetpointService : public mb::rpc::BaseService<ichnaea_SetpointRequest, ichnaea_SetpointResponse>
  {
  public:
    SetpointService() :
        BaseService<ichnaea_SetpointRequest, ichnaea_SetpointResponse>( "SetpointService", ichnaea_Service_SVC_SETPOINT,
                                                                        ichnaea_Message_MSG_SETPOINT_REQ,
                                                                        ichnaea_Message_MSG_SETPOINT_RSP ) {};
    ~SetpointService() = default;

    /**
     * @copydoc IService::processRequest
     */
    mb::rpc::ErrId processRequest() final override;
  };


  class SensorService : public mb::rpc::BaseService<ichnaea_SensorRequest, ichnaea_SensorResponse>
  {
  public:
    SensorService() :
        BaseService<ichnaea_SensorRequest, ichnaea_SensorResponse>(
            "SensorService", ichnaea_Service_SVC_SENSOR, ichnaea_Message_MSG_SENSOR_REQ, ichnaea_Message_MSG_SENSOR_RSP ) {};
    ~SensorService() = default;

    /**
     * @copydoc IService::processRequest
     */
    mb::rpc::ErrId processRequest() final override;
  };
}    // namespace COM::RPC

#endif /* !ICHNAEA_RPC_SERVICES_HPP */
