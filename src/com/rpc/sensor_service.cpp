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
        response.value = getMeasurement( Element::VMON_SOLAR_INPUT );
        break;

      case ichnaea_SensorType_SENSOR_OUTPUT_VOLTAGE:
        response.value = getMeasurement( Element::VMON_BATT_OUTPUT );
        break;

      case ichnaea_SensorType_SENSOR_AVG_OUTPUT_CURRENT:
        response.value = getMeasurement( Element::IMON_LTC_AVG );
        break;

      case ichnaea_SensorType_SENSOR_BOARD_TEMP_1:
        response.value = getMeasurement( Element::RP2040_TEMP );
        break;

      case ichnaea_SensorType_SENSOR_BOARD_TEMP_2:
        response.value = getMeasurement( Element::BOARD_TEMP_0 );
        break;

      case ichnaea_SensorType_SENSOR_BOARD_TEMP_3:
        response.value = getMeasurement( Element::BOARD_TEMP_1 );
        break;

      case ichnaea_SensorType_SENSOR_CHARGE_CURRENT:
        response.value = getMeasurement( Element::IMON_BATT );
        break;

      case ichnaea_SensorType_SENSOR_VMON_1V1:
        response.value = getMeasurement( Element::VMON_1V1 );
        break;

      case ichnaea_SensorType_SENSOR_VMON_3V3:
        response.value = getMeasurement( Element::VMON_3V3 );
        break;

      case ichnaea_SensorType_SENSOR_VMON_5V:
        response.value = getMeasurement( Element::VMON_5V0 );
        break;

      case ichnaea_SensorType_SENSOR_VMON_12V:
        response.value = getMeasurement( Element::VMON_12V );
        break;

      default:
        response.status = ichnaea_SensorError_ERR_SENSOR_NOT_SUPPORTED;
    }

    return mbed_rpc_ErrorCode_ERR_NO_ERROR;
  }
}  // namespace COM::RPC
