#include <mbedutils/assert.hpp>
#include <mbedutils/database.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/pdi/target_system_current_output.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  bool setTargetSystemCurrentOutput( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.targetSystemCurrentOutput ) );
    return write( KEY_TARGET_SYSTEM_CURRENT_OUTPUT, &value, sizeof( value ) ) == sizeof( value );
  }

  float getTargetSystemCurrentOutput()
  {
    float value = 0.0f;
    read( KEY_TARGET_SYSTEM_CURRENT_OUTPUT, &value, sizeof( value ) );
    return value;
  }

  static void onWrite__target_system_current_output( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::IMON_LOAD );
  }

  static void sanitize__target_system_current_output( mb::db::KVNode &node, void *data, const size_t size )
  {
    ( void )size;

    mbed_dbg_assert( node.hashKey == PDI::KEY_TARGET_SYSTEM_CURRENT_OUTPUT );

    /*-------------------------------------------------------------------------
    Ensure that we don't set the current limit below 0.5A. This is more to
    ensure the monitors have a reasonable range to work with that accounts for
    sensor noise and precision.
    -------------------------------------------------------------------------*/
    ichnaea_PDI_FloatConfiguration *msg = static_cast<ichnaea_PDI_FloatConfiguration *>( data );
    if( msg->value < 0.5f )
    {
      msg->value = 0.5f;
    }
  }

  void pdi_register_key__target_system_current_output()
  {
    using namespace App;
    using namespace mb::db;
    using namespace System::Database;

    // This sets it to 1A per phase
    PDI::Internal::RAMCache.targetSystemCurrentOutput = 6.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_TARGET_SYSTEM_CURRENT_OUTPUT;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.targetSystemCurrentOutput;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_VOLATILE;
    node.onWrite   = VisitorFunc::create<onWrite__target_system_current_output>();
    node.sanitizer = SanitizeFunc::create<sanitize__target_system_current_output>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
