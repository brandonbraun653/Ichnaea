/******************************************************************************
 *  File Name:
 *    sensor_service.cpp
 *
 *  Description:
 *    Implements the sensor RPC service
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/

#include "src/com/rpc/rpc_services.hpp"
#include "src/system/system_sensor.hpp"
#include "src/system/system_util.hpp"
#include "src/version.hpp"

namespace COM::RPC
{
  /*---------------------------------------------------------------------------
  Service Implementations
  ---------------------------------------------------------------------------*/

  mb::rpc::ErrId SensorService::processRequest()
  {
    using namespace Sensor;

    /*-------------------------------------------------------------------------
    Ignore requests that are not intended for this node
    -------------------------------------------------------------------------*/
    if( request.node_id != System::identity() )
    {
      return mbed_rpc_ErrorCode_ERR_SVC_NO_RSP;
    }

    /*-------------------------------------------------------------------------
    Default initialize the response
    -------------------------------------------------------------------------*/
    response.status = ichnaea_SensorError_ERR_SENSOR_NO_ERROR;

    switch( request.sensor )
    {
      case ichnaea_SensorType_SENSOR_INPUT_VOLTAGE:
        response.value = getHighSideVoltage();
        break;

      case ichnaea_SensorType_SENSOR_OUTPUT_VOLTAGE:
        response.value = getLowSideVoltage();
        break;

      case ichnaea_SensorType_SENSOR_AVG_OUTPUT_CURRENT:
        response.value = getAverageCurrent();
        break;

      case ichnaea_SensorType_SENSOR_BOARD_TEMP_1:
        response.value = getRP2040Temp();
        break;

      case ichnaea_SensorType_SENSOR_BOARD_TEMP_2:
        response.value = getBoardTemp0();
        break;

      case ichnaea_SensorType_SENSOR_BOARD_TEMP_3:
        response.value = getBoardTemp1();
        break;

      default:
        response.status = ichnaea_SensorError_ERR_SENSOR_NOT_SUPPORTED;
    }

    return mbed_rpc_ErrorCode_ERR_NO_ERROR;
  }
}  // namespace COM::RPC
