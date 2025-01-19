#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/pdi/boot_count.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  bool setBootCount( uint32_t value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.bootCount ) );
    return write( KEY_BOOT_COUNT, &value, sizeof( value ) ) == sizeof( value );
  }

  uint32_t getBootCount()
  {
    uint32_t value = 0;
    read( KEY_BOOT_COUNT, &value, sizeof( value ) );
    return value;
  }


  void pdi_register_key_boot_count()
  {
    using namespace App;
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    PDI::Internal::RAMCache.bootCount = 0;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_BOOT_COUNT;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.bootCount;
    node.dataSize  = ichnaea_PDI_BootCount_size;
    node.pbFields  = ichnaea_PDI_BootCount_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
