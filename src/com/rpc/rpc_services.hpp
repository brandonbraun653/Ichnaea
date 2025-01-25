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
#include "mbedutils/drivers/rpc/rpc_common.hpp"
#ifndef ICHNAEA_RPC_SERVICES_HPP
#define ICHNAEA_RPC_SERVICES_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <src/app/proto/ichnaea_rpc.pb.h>
#include <src/system/system_util.hpp>
#include <mbedutils/rpc.hpp>

namespace COM::RPC
{
  /*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/

  class PingNodeService : public mb::rpc::service::BaseService<ichnaea_PingNodeRequest, ichnaea_PingNodeResponse>
  {
  public:
    PingNodeService() :
        BaseService<ichnaea_PingNodeRequest, ichnaea_PingNodeResponse>( "PingNodeService", ichnaea_Service_SVC_PING_NODE,
                                                                        ichnaea_Message_MSG_PING_NODE_REQ,
                                                                        ichnaea_Message_MSG_PING_NODE_RSP ){};
    ~PingNodeService() = default;

    /**
     * @copydoc IService::processRequest
     */
    mb::rpc::ErrId processRequest() final override;
  };

  class IdentityService : public mb::rpc::service::BaseService<ichnaea_GetIdRequest, ichnaea_GetIdResponse>
  {
  public:
    IdentityService() :
        BaseService<ichnaea_GetIdRequest, ichnaea_GetIdResponse>(
            "IdentityService", ichnaea_Service_SVC_IDENTITY, ichnaea_Message_MSG_GET_ID_REQ, ichnaea_Message_MSG_GET_ID_RSP ){};
    ~IdentityService() = default;

    /**
     * @copydoc IService::processRequest
     */
    mb::rpc::ErrId processRequest() final override;
  };


  class ManagerService : public mb::rpc::service::BaseService<ichnaea_ManagerRequest, ichnaea_ManagerResponse>
  {
  public:
    ManagerService() :
        BaseService<ichnaea_ManagerRequest, ichnaea_ManagerResponse>(
            "ManagerService", ichnaea_Service_SVC_MANAGER, ichnaea_Message_MSG_MANAGER_REQ, ichnaea_Message_MSG_MANAGER_RSP ){};
    ~ManagerService() = default;

    /**
     * @copydoc IService::processRequest
     */
    mb::rpc::ErrId processRequest() final override;

  private:
    mb::rpc::ErrId reboot();
    mb::rpc::ErrId engage_output();
    mb::rpc::ErrId disengage_output();
  };

  class SetpointService : public mb::rpc::service::BaseService<ichnaea_SetpointRequest, ichnaea_SetpointResponse>
  {
  public:
    SetpointService() :
        BaseService<ichnaea_SetpointRequest, ichnaea_SetpointResponse>( "SetpointService", ichnaea_Service_SVC_SETPOINT,
                                                                        ichnaea_Message_MSG_SETPOINT_REQ,
                                                                        ichnaea_Message_MSG_SETPOINT_RSP ){};
    ~SetpointService() = default;

    /**
     * @copydoc IService::processRequest
     */
    mb::rpc::ErrId processRequest() final override;
  };


  class SensorService : public mb::rpc::service::BaseService<ichnaea_SensorRequest, ichnaea_SensorResponse>
  {
  public:
    SensorService() :
        BaseService<ichnaea_SensorRequest, ichnaea_SensorResponse>(
            "SensorService", ichnaea_Service_SVC_SENSOR, ichnaea_Message_MSG_SENSOR_REQ, ichnaea_Message_MSG_SENSOR_RSP ){};
    ~SensorService() = default;

    /**
     * @copydoc IService::processRequest
     */
    mb::rpc::ErrId processRequest() final override;
  };


  class PDIReadService : public mb::rpc::service::BaseService<ichnaea_PDIReadRequest, ichnaea_PDIReadResponse>
  {
  public:
    PDIReadService() :
        BaseService<ichnaea_PDIReadRequest, ichnaea_PDIReadResponse>( "PDIReadService", ichnaea_Service_SVC_PDI_READ,
                                                                      ichnaea_Message_MSG_PDI_READ_REQ,
                                                                      ichnaea_Message_MSG_PDI_READ_RSP ){};
    ~PDIReadService() = default;

    /**
     * @copydoc IService::processRequest
     */
    mb::rpc::ErrId processRequest() final override;
  };


  class PDIWriteService : public mb::rpc::service::BaseService<ichnaea_PDIWriteRequest, ichnaea_PDIWriteResponse>
  {
  public:
    PDIWriteService() :
        BaseService<ichnaea_PDIWriteRequest, ichnaea_PDIWriteResponse>( "PDIWriteService", ichnaea_Service_SVC_PDI_WRITE,
                                                                        ichnaea_Message_MSG_PDI_WRITE_REQ,
                                                                        ichnaea_Message_MSG_PDI_WRITE_RSP ){};
    ~PDIWriteService() = default;

    /**
     * @copydoc IService::processRequest
     */
    mb::rpc::ErrId processRequest() final override;
  };


  class SystemStatusService : public mb::rpc::service::BaseService<ichnaea_SystemStatusRequest, ichnaea_SystemStatusResponse>
  {
  public:
    SystemStatusService() :
        BaseService<ichnaea_SystemStatusRequest, ichnaea_SystemStatusResponse>(
            "SystemStatusService", ichnaea_Service_SVC_SYSTEM_STATUS, ichnaea_Message_MSG_SYSTEM_STATUS_REQ,
            ichnaea_Message_MSG_SYSTEM_STATUS_RSP ){};
    ~SystemStatusService() = default;

    /**
     * @copydoc IService::processRequest
     */
    mb::rpc::ErrId processRequest() final override;
  };

}    // namespace COM::RPC

#endif /* !ICHNAEA_RPC_SERVICES_HPP */
