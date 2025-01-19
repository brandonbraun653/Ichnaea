#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/pdi/config_min_temp_limit.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  bool setConfigMinTempLimit( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.configMinTempLimit ) );
    return write( KEY_CONFIG_MIN_TEMP_LIMIT, &value, sizeof( value ) ) == sizeof( value );
  }

  float getConfigMinTempLimit()
  {
    float value = 0.0f;
    read( KEY_CONFIG_MIN_TEMP_LIMIT, &value, sizeof( value ) );
    return value;
  }

  static void onWrite__config_min_temp_limit( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::BOARD_TEMP_0 );
  }

  void pdi_register_key__config_min_temp_limit()
  {
    using namespace App;
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    PDI::Internal::RAMCache.configMinTempLimit = -40.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_CONFIG_MIN_TEMP_LIMIT;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.configMinTempLimit;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = VisitorFunc::create<onWrite__config_min_temp_limit>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
