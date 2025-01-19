#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/app_power.hpp>
#include <src/app/pdi/target_system_voltage_output.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  bool setTargetSystemVoltageOutput( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.targetSystemVoltageOutput ) );
    return write( KEY_TARGET_SYSTEM_VOLTAGE_OUTPUT, &value, sizeof( value ) ) == sizeof( value );
  }

  float getTargetSystemVoltageOutput()
  {
    float value = 0.0f;
    read( KEY_TARGET_SYSTEM_VOLTAGE_OUTPUT, &value, sizeof( value ) );
    return value;
  }

  static void onWrite__target_system_voltage_output( mb::db::KVNode &node )
  {
    /*-------------------------------------------------------------------------
    Drive the HW setpoint to the new value
    -------------------------------------------------------------------------*/
    ichnaea_PDI_FloatConfiguration *msg = static_cast<ichnaea_PDI_FloatConfiguration *>( node.datacache );
    App::Power::setOutputVoltage( msg->value );

    /*-------------------------------------------------------------------------
    Reset the monitor to account for the new voltage output
    -------------------------------------------------------------------------*/
    Monitor::refreshPDIDependencies( System::Sensor::Element::VMON_LOAD );
  }

  void pdi_register_key__target_system_voltage_output()
  {
    using namespace App;
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    PDI::Internal::RAMCache.targetSystemVoltageOutput = 0.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_TARGET_SYSTEM_VOLTAGE_OUTPUT;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.targetSystemVoltageOutput;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = VisitorFunc::create<onWrite__target_system_voltage_output>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
