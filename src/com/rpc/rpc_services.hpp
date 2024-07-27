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

  class IdentityService : public mb::rpc::BaseService<ichnaea_GetIdRequest, ichnaea_GetIdResponse>
  {
  public:
    IdentityService() :
        BaseService<ichnaea_GetIdRequest, ichnaea_GetIdResponse>( "IdentityService", ichnaea_Service_SVC_IDENTITY,
                                                                 ichnaea_Message_MSG_GET_ID_REQ,
                                                                 ichnaea_Message_MSG_GET_ID_RSP ){};
    ~IdentityService() = default;

    /**
     * @copydoc IService::processRequest
     */
    mb::rpc::ErrId processRequest() final override;
  };
}  // namespace COM::R

#endif  /* !ICHNAEA_RPC_SERVICES_HPP */
