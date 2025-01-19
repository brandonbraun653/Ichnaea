#include "mbedutils/drivers/database/db_kv_node.hpp"
#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/app_power.hpp>
#include <src/app/pdi/target_phase_current_output.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  bool setTargetPhaseCurrentOutput( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.targetPhaseCurrentOutput ) );
    return write( KEY_TARGET_PHASE_CURRENT_OUTPUT, &value, sizeof( value ) ) == sizeof( value );
  }

  float getTargetPhaseCurrentOutput()
  {
    float value = 0.0f;
    read( KEY_TARGET_PHASE_CURRENT_OUTPUT, &value, sizeof( value ) );
    return value;
  }

  static void onWrite__target_phase_current_output( mb::db::KVNode &node )
  {
    ( void )node;
    Monitor::refreshPDIDependencies( System::Sensor::Element::IMON_LOAD );
  }

  static void sanitize__target_phase_current_output( mb::db::KVNode &node, void *data, const size_t size )
  {
    ( void )size;

    mbed_dbg_assert( node.hashKey == PDI::KEY_TARGET_PHASE_CURRENT_OUTPUT );
    ichnaea_PDI_FloatConfiguration *d = static_cast<ichnaea_PDI_FloatConfiguration *>( data );

    if( d->value < 0.1f )
    {
      d->value = 0.1f;
    }
  }

  void pdi_register_key__target_phase_current_output()
  {
    using namespace App;
    using namespace mb::db;
    using namespace System::Database;

    // 1A is a fairly safe default value
    PDI::Internal::RAMCache.targetPhaseCurrentOutput = 1.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_TARGET_PHASE_CURRENT_OUTPUT;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.targetPhaseCurrentOutput;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = VisitorFunc::create<onWrite__target_phase_current_output>();
    node.sanitizer = SanitizeFunc::create<sanitize__target_phase_current_output>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
