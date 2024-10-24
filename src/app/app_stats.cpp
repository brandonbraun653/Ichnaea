/******************************************************************************
 *  File Name:
 *    app_stats.cpp
 *
 *  Description:
 *    Ichnaea application statistics tracking implementation
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/assert.hpp>
#include <mbedutils/logging.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/app_stats.hpp>
#include <src/system/system_db.hpp>

namespace App::Stats
{
  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/

  static void pdi_register_key_boot_count()
  {
    using namespace mb::db;
    using namespace System::Database;

    /*-------------------------------------------------------------------------
    Register the boot count key with the PDI database
    -------------------------------------------------------------------------*/
    PDI::Internal::RAMCache.bootCount = 0;

    KVNode node;
    node.hashKey   = PDI::KEY_BOOT_COUNT;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.bootCount;
    node.dataSize  = ichnaea_PDI_BootCount_size;
    node.pbFields  = ichnaea_PDI_BootCount_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;

    mbed_assert( pdi_insert_and_create( node, node.datacache, node.dataSize ) );
    System::Database::pdiDB().sync( node.hashKey );
  }

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    /*-------------------------------------------------------------------------
    Register the PDI keys for the statistics tracking system
    -------------------------------------------------------------------------*/
    pdi_register_key_boot_count();

    /*-------------------------------------------------------------------------
    Read out the boot count and increment it
    -------------------------------------------------------------------------*/
    uint32_t boot_count = 0;
    System::Database::pdiDB().read( PDI::KEY_BOOT_COUNT, &boot_count, sizeof( boot_count ) );
    boot_count++;
    System::Database::pdiDB().write( PDI::KEY_BOOT_COUNT, &boot_count, sizeof( boot_count ) );
    System::Database::pdiDB().flush();

    LOG_INFO( "Boot Count = %d", boot_count );
  }
}  // namespace App::Stats
