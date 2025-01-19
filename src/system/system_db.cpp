/******************************************************************************
 *  File Name:
 *    system_db.cpp
 *
 *  Description:
 *    Ichnaea program configuration settings interface implementation
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/assert.hpp>
#include <mbedutils/database.hpp>
#include <mbedutils/logging.hpp>
#include <mbedutils/util.hpp>
#include <src/integration/flashdb/fal_cfg.h>
#include <src/system/system_db.hpp>
#include <src/system/system_error.hpp>
#include <src/hw/nor.hpp>
#include "fal_def.h"
#include "mbedutils/drivers/logging/logging_macros.hpp"

namespace System::Database
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  static constexpr size_t PDI_MAX_COUNT      = 100;
  static constexpr size_t PDI_TRANSCODE_SIZE = 512;

  /*---------------------------------------------------------------------------
  Public Data
  ---------------------------------------------------------------------------*/

  /**
   * @brief FlashDB descriptor for the NOR flash device
   */
  extern "C" const struct fal_flash_dev fdb_nor_flash0 = {
    .name       = ICHNAEA_DB_FLASH_DEV_NAME,
    .addr       = HW::NOR::FLASH_ADDR_MIN,
    .len        = HW::NOR::FLASH_ADDR_MAX,
    .blk_size   = HW::NOR::ERASE_BLOCK_SIZE,
    .ops        = { .init = HW::NOR::init, .read = HW::NOR::read, .write = HW::NOR::write, .erase = HW::NOR::erase },
    .write_gran = 1
  };

  /*---------------------------------------------------------------------------
  Private Data
  ---------------------------------------------------------------------------*/

  static size_t                                             s_db_ready;
  static mb::db::NvmKVDB                                    s_pdi_kvdb;
  static mb::db::Storage<PDI_MAX_COUNT, PDI_TRANSCODE_SIZE> s_kvdb_storage;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    using namespace mb;
    using namespace mb::db;

    if( s_db_ready == DRIVER_INITIALIZED_KEY )
    {
      return;
    }

    /*-------------------------------------------------------------------------
    Configure the NVM based KVDB driver
    -------------------------------------------------------------------------*/
    NvmKVDB::Config cfg;

    cfg.dev_name             = ICHNAEA_DB_FLASH_DEV_NAME;
    cfg.part_name            = ICHNAEA_DB_PDI_RGN_NAME;
    cfg.dev_sector_size      = HW::NOR::ERASE_BLOCK_SIZE;
    cfg.ext_node_dsc         = &s_kvdb_storage.node_dsc;
    cfg.ext_transcode_buffer = s_kvdb_storage.transcode_buffer;

    if( s_pdi_kvdb.configure( cfg ) != DB_ERR_NONE )
    {
      Panic::throwError( Panic::ErrorCode::ERR_SYSTEM_INIT_FAIL );
      return;
    }

    /*-------------------------------------------------------------------------
    Power on the KVDB driver. After this call the system should be ready.
    -------------------------------------------------------------------------*/
    bool success = s_pdi_kvdb.init();
    if( !success )
    {
      LOG_DEBUG( "Initial PDI database init failed. Resetting to factory defaults." );
      s_pdi_kvdb.deinit();

      /*-----------------------------------------------------------------------
      Erase the entire PDI region to ensure a clean slate
      -----------------------------------------------------------------------*/
      static constexpr size_t BIG_ERASE_BLOCK_SIZE = 64 * 1024;
      static_assert( ( ICHNAEA_DB_PDI_RGN_SIZE % BIG_ERASE_BLOCK_SIZE ) == 0, "PDI region size must be a multiple of 64k" );

      for( size_t address = ICHNAEA_DB_PDI_RGN_START; address < ICHNAEA_DB_PDI_RGN_SIZE; address += BIG_ERASE_BLOCK_SIZE )
      {
        LOG_DEBUG( "Erasing 64kB @ addr: 0x%08X", address );
        HW::NOR::erase( address, BIG_ERASE_BLOCK_SIZE );
      }

      /*-----------------------------------------------------------------------
      Attempt to reinitialize the database. If this fails, the system is in a
      degraded state and needs to be investigated.
      -----------------------------------------------------------------------*/
      LOG_DEBUG( "Re-init the PDI database. This can take a few seconds." );
      if( !s_pdi_kvdb.init() )
      {
        mbed_assert_continue_msg( false, "PDI database init permanently disabled." );
        return;
      }
      LOG_DEBUG( "Success" );
    }

    s_db_ready = DRIVER_INITIALIZED_KEY;
  }


  mb::db::NvmKVDB &pdiDB()
  {
    return s_pdi_kvdb;
  }


  void pdi_insert_and_create( mb::db::KVNode &node, void *dflt_data, const size_t size )
  {
    using namespace mb;
    using namespace mb::db;

    /*-------------------------------------------------------------------------
    Basic sanity checks on the input parameters/state
    -------------------------------------------------------------------------*/
    mbed_dbg_assert( dflt_data != nullptr );
    mbed_dbg_assert( size > 0 );
    mbed_dbg_assert( s_db_ready == DRIVER_INITIALIZED_KEY );

    /*-------------------------------------------------------------------------
    Insert the node into the database
    -------------------------------------------------------------------------*/
    mbed_assert_msg( s_pdi_kvdb.insert( node ), "PDI key %d insert fail", node.hashKey );

    /*-------------------------------------------------------------------------
    Write the default data to the database if it doesn't already exist
    -------------------------------------------------------------------------*/
    if( !s_pdi_kvdb.exists( node.hashKey ) )
    {
      mbed_assert_msg( s_pdi_kvdb.write( node.hashKey, dflt_data, size ) == DB_ERR_NONE, "PDI key %d dflt write fail", node.hashKey );
    }

    /*-------------------------------------------------------------------------
    Sync the data from NVM if it's a persistent key
    -------------------------------------------------------------------------*/
    if( node.flags & KV_FLAG_DEFAULT_PERSISTENT )
    {
      s_pdi_kvdb.sync( node.hashKey );
    }
  }
}    // namespace System::Database
