/******************************************************************************
 *  File Name:
 *    ltc7871_services.cpp
 *
 *  Description:
 *    Services for inspecting/controlling the LTC7871 power management IC
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "src/com/rpc/rpc_services.hpp"
#include "src/hw/ltc7871.hpp"
#include "src/hw/ltc7871_prv.hpp"
#include "src/hw/ltc7871_reg.hpp"

namespace COM::RPC
{
  mb::rpc::ErrId LTCRegisterGetService::processRequest()
  {
    /*-------------------------------------------------------------------------
    Ignore the message if it's not for us
    -------------------------------------------------------------------------*/
    if( request.node_id != System::identity() )
    {
      return mbed_rpc_ErrorCode_ERR_SVC_NO_RSP;
    }

    /*-------------------------------------------------------------------------
    Validate the input range
    -------------------------------------------------------------------------*/
    if( ( request.reg < HW::LTC7871::REG_MIN_ADDR ) || ( request.reg > HW::LTC7871::REG_MAX_ADDR ) )
    {
      response.status = ichnaea_LTCRegisterError_ERR_LTC_REG_BAD_VALUE;
      return mbed_rpc_ErrorCode_ERR_NO_ERROR;
    }

    /*-------------------------------------------------------------------------
    Process the request
    -------------------------------------------------------------------------*/
    response.status = ichnaea_LTCRegisterError_ERR_LTC_ACCESS_FAILED;
    response.value  = 0;

    if( HW::LTC7871::available() )
    {
      response.value  = HW::LTC7871::Private::read_register( request.reg );
      response.status = ichnaea_LTCRegisterError_ERR_LTC_REG_NO_ERROR;
    }

    return mbed_rpc_ErrorCode_ERR_NO_ERROR;
  }


  mb::rpc::ErrId LTCRegisterSetService::processRequest()
  {
    /*-------------------------------------------------------------------------
    Ignore the message if it's not for us
    -------------------------------------------------------------------------*/
    if( request.node_id != System::identity() )
    {
      return mbed_rpc_ErrorCode_ERR_SVC_NO_RSP;
    }

    /*-------------------------------------------------------------------------
    Validate the input range
    -------------------------------------------------------------------------*/
    if( ( request.reg < HW::LTC7871::REG_MIN_ADDR ) || ( request.reg > HW::LTC7871::REG_MAX_ADDR ) )
    {
      response.status = ichnaea_LTCRegisterError_ERR_LTC_REG_BAD_VALUE;
      return mbed_rpc_ErrorCode_ERR_NO_ERROR;
    }

    if( request.reg < HW::LTC7871::REG_MFR_CHIP_CTRL )
    {
      response.status = ichnaea_LTCRegisterError_ERR_LTC_REG_READ_ONLY;
      return mbed_rpc_ErrorCode_ERR_NO_ERROR;
    }

    /*-------------------------------------------------------------------------
    Process the request
    -------------------------------------------------------------------------*/
    response.status = ichnaea_LTCRegisterError_ERR_LTC_ACCESS_FAILED;

    if( HW::LTC7871::available() )
    {
      HW::LTC7871::Private::idac_write_protect( false );
      HW::LTC7871::Private::write_register( request.reg, request.value );
      HW::LTC7871::Private::idac_write_protect( true );
      response.status = ichnaea_LTCRegisterError_ERR_LTC_REG_NO_ERROR;
    }

    return mbed_rpc_ErrorCode_ERR_NO_ERROR;
  }
}    // namespace COM::RPC
