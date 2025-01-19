#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/pdi/min_system_voltage_input_rated_limit.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  bool setMinSystemVoltageInputRatedLimit( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.minSystemVoltageInputRatedLimit ) );
    return write( KEY_CONFIG_MIN_SYSTEM_VOLTAGE_INPUT_RATED_LIMIT, &value, sizeof( value ) ) == sizeof( value );
  }

  float getMinSystemVoltageInputRatedLimit()
  {
    float value = 0.0f;
    read( KEY_CONFIG_MIN_SYSTEM_VOLTAGE_INPUT_RATED_LIMIT, &value, sizeof( value ) );
    return value;
  }

  static void onWrite__min_system_voltage_input_rated_limit( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::VMON_LOAD );
  }

  void pdi_register_key__min_system_voltage_input_rated_limit()
  {
    using namespace App;
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    PDI::Internal::RAMCache.minSystemVoltageInputRatedLimit = 10.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_CONFIG_MIN_SYSTEM_VOLTAGE_INPUT_RATED_LIMIT;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.minSystemVoltageInputRatedLimit;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = VisitorFunc::create<onWrite__min_system_voltage_input_rated_limit>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
