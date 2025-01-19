#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/app_power.hpp>
#include <src/app/pdi/phase_current_output_rated_limit.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  bool setPhaseCurrentOutputRatedLimit( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.phaseCurrentOutputRatedLimit ) );
    return write( KEY_CONFIG_PHASE_CURRENT_OUTPUT_RATED_LIMIT, &value, sizeof( value ) ) == sizeof( value );
  }

  float getPhaseCurrentOutputRatedLimit()
  {
    float value = 0.0f;
    read( KEY_CONFIG_PHASE_CURRENT_OUTPUT_RATED_LIMIT, &value, sizeof( value ) );
    return value;
  }

  void pdi_register_key__phase_current_output_rated_limit()
  {
    using namespace App;
    using namespace mb::db;
    using namespace System::Database;

    // Inductors saturate at ~32A, so we must stay below this.
    PDI::Internal::RAMCache.phaseCurrentOutputRatedLimit = 25.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_CONFIG_PHASE_CURRENT_OUTPUT_RATED_LIMIT;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.phaseCurrentOutputRatedLimit;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}
