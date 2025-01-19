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
#include <src/app/pdi/mon_12v0_voltage.hpp>
#include <src/app/pdi/mon_1v1_voltage.hpp>
#include <src/app/pdi/mon_3v3_voltage.hpp>
#include <src/app/pdi/mon_5v0_voltage.hpp>
#include <src/app/pdi/mon_input_voltage.hpp>
#include <src/app/pdi/mon_output_current.hpp>
#include <src/app/pdi/mon_output_voltage.hpp>
#include <src/app/proto/ichnaea_rpc.pb.h>
#include <src/com/rpc/rpc_services.hpp>
#include <src/system/system_sensor.hpp>
#include <src/system/system_util.hpp>
#include <src/version.hpp>

namespace COM::RPC
{
  /*---------------------------------------------------------------------------
  Service Implementations
  ---------------------------------------------------------------------------*/

  mb::rpc::ErrId SensorService::processRequest()
  {
    using namespace System::Sensor;

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
        response.value = App::PDI::getMonInputVoltageFiltered();
        break;

      case ichnaea_SensorType_SENSOR_OUTPUT_VOLTAGE:
        response.value = App::PDI::getMonOutputVoltageFiltered();
        break;

      case ichnaea_SensorType_SENSOR_LTC_AVG_OUTPUT_CURRENT:
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

      case ichnaea_SensorType_SENSOR_OUTPUT_CURRENT:
        response.value = App::PDI::getMonOutputCurrentFiltered();
        break;

      case ichnaea_SensorType_SENSOR_VOLTAGE_MON_1V1:
        response.value = App::PDI::getMon1V1VoltageFiltered();
        break;

      case ichnaea_SensorType_SENSOR_VOLTAGE_MON_3V3:
        response.value = App::PDI::getMon3V3VoltageFiltered();
        break;

      case ichnaea_SensorType_SENSOR_VOLTAGE_MON_5V:
        response.value = App::PDI::getMon5V0VoltageFiltered();
        break;

      case ichnaea_SensorType_SENSOR_VOLTAGE_MON_12V:
        response.value = App::PDI::getMon12V0VoltageFiltered();
        break;

      default:
        response.status = ichnaea_SensorError_ERR_SENSOR_NOT_SUPPORTED;
    }

    return mbed_rpc_ErrorCode_ERR_NO_ERROR;
  }
}    // namespace COM::RPC
