#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/pdi/config_max_system_voltage_input.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  bool setConfigMaxSystemVoltageInput( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.maxSystemVoltageInput ) );
    return write( KEY_CONFIG_MAX_SYSTEM_VOLTAGE_INPUT, &value, sizeof( value ) ) == sizeof( value );
  }

  float getConfigMaxSystemVoltageInput()
  {
    float value = 0.0f;
    read( KEY_CONFIG_MAX_SYSTEM_VOLTAGE_INPUT, &value, sizeof( value ) );
    return value;
  }

  static void onWrite__config_max_system_voltage_input( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::VMON_SOLAR_INPUT );
  }

  void pdi_register_key__config_max_system_voltage_input()
  {
    using namespace App;
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    PDI::Internal::RAMCache.maxSystemVoltageInput = 90.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_CONFIG_MAX_SYSTEM_VOLTAGE_INPUT;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.maxSystemVoltageInput;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = VisitorFunc::create<onWrite__config_max_system_voltage_input>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
