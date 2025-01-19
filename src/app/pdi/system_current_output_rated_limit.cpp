#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/app_power.hpp>
#include <src/app/pdi/system_current_output_rated_limit.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  bool setSystemCurrentOutputRatedLimit( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.systemCurrentOutputRatedLimit ) );
    return write( KEY_CONFIG_SYSTEM_CURRENT_OUTPUT_RATED_LIMIT, &value, sizeof( value ) ) == sizeof( value );
  }

  float getSystemCurrentOutputRatedLimit()
  {
    float value = 0.0f;
    read( KEY_CONFIG_SYSTEM_CURRENT_OUTPUT_RATED_LIMIT, &value, sizeof( value ) );
    return value;
  }

  static void onWrite__system_current_output_rated_limit( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::IMON_LOAD );
  }

  void pdi_register_key__system_current_output_rated_limit()
  {
    using namespace App;
    using namespace mb::db;
    using namespace System::Database;

    // As of V2, the board should be able to handle 150A
    PDI::Internal::RAMCache.systemCurrentOutputRatedLimit = 150.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_CONFIG_SYSTEM_CURRENT_OUTPUT_RATED_LIMIT;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.systemCurrentOutputRatedLimit;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = VisitorFunc::create<onWrite__system_current_output_rated_limit>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
