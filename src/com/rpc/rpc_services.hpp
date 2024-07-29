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
#include "src/system/system_util.hpp"
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

  private:
    mb::rpc::ErrId set_voltage_output( const float voltage );
    mb::rpc::ErrId set_max_current_limit( const float current );
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


  class LTCRegisterGetService : public mb::rpc::BaseService<ichnaea_LTCRegisterGetRequest, ichnaea_LTCRegisterGetResponse>
  {
  public:
    LTCRegisterGetService() :
        BaseService<ichnaea_LTCRegisterGetRequest, ichnaea_LTCRegisterGetResponse>(
            "LTCRegisterGetService", ichnaea_Service_SVC_LTC_REG_GET, ichnaea_Message_MSG_LTC_REG_GET_REQ,
            ichnaea_Message_MSG_LTC_REG_GET_RSP ) {};
    ~LTCRegisterGetService() = default;

    /**
     * @copydoc IService::processRequest
     */
    mb::rpc::ErrId processRequest() final override;
  };


  class LTCRegisterSetService : public mb::rpc::BaseService<ichnaea_LTCRegisterSetRequest, ichnaea_LTCRegisterSetResponse>
  {
  public:
    LTCRegisterSetService() :
        BaseService<ichnaea_LTCRegisterSetRequest, ichnaea_LTCRegisterSetResponse>(
            "LTCRegisterSetService", ichnaea_Service_SVC_LTC_REG_SET, ichnaea_Message_MSG_LTC_REG_SET_REQ,
            ichnaea_Message_MSG_LTC_REG_SET_RSP ) {};
    ~LTCRegisterSetService() = default;

    /**
     * @copydoc IService::processRequest
     */
    mb::rpc::ErrId processRequest() final override;
  };
}    // namespace COM::RPC

#endif /* !ICHNAEA_RPC_SERVICES_HPP */
