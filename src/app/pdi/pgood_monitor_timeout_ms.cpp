#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/pdi/pgood_monitor_timeout_ms.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  bool setPgoodMonitorTimeoutMS( uint32_t value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.pgoodMonitorTimeoutMS ) );
    return write( KEY_PGOOD_MONITOR_TIMEOUT_MS, &value, sizeof( value ) ) == sizeof( value );
  }

  uint32_t getPgoodMonitorTimeoutMS()
  {
    uint32_t value = 0;
    read( KEY_PGOOD_MONITOR_TIMEOUT_MS, &value, sizeof( value ) );
    return value;
  }

  static void onWrite__pgood_monitor_timeout_ms( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::VMON_LOAD );
  }

  void pdi_register_key__pgood_monitor_timeout_ms()
  {
    using namespace App;
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    PDI::Internal::RAMCache.pgoodMonitorTimeoutMS = 50;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_PGOOD_MONITOR_TIMEOUT_MS;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.pgoodMonitorTimeoutMS;
    node.dataSize  = ichnaea_PDI_Uint32Configuration_size;
    node.pbFields  = ichnaea_PDI_Uint32Configuration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = VisitorFunc::create<onWrite__pgood_monitor_timeout_ms>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
