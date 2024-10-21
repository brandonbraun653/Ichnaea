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

  static size_t                                                      s_db_ready;
  static mb::db::NvmKVDB                                             s_pdi_kvdb;
  // static mb::db::NvmKVDB::Storage<PDI_MAX_COUNT, PDI_TRANSCODE_SIZE> s_kvdb_storage;

  static mb::db::RamKVDB                         s_kv_ram_db;        /**< RAM manager for KV pair cache */
  static mb::db::KVNodeVector<PDI_MAX_COUNT>     s_kv_nodes;         /**< Storage for KV pair descriptors */
  static etl::array<uint8_t, PDI_TRANSCODE_SIZE> s_transcode_buffer; /**< Storage for encoding/decoding largest data */


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
    Configure the RAM based KVDB driver (subcomponent of NVM driver)
    -------------------------------------------------------------------------*/
    RamKVDB::Config ram_cfg;
    ram_cfg.node_storage     = &s_kv_nodes;
    ram_cfg.transcode_buffer = s_transcode_buffer;

    if( s_kv_ram_db.configure( ram_cfg ) != DB_ERR_NONE )
    {
      Panic::throwError( Panic::ErrorCode::ERR_SYSTEM_INIT_FAIL );
      return;
    }

    /*-------------------------------------------------------------------------
    Configure the NVM based KVDB driver
    -------------------------------------------------------------------------*/
    NvmKVDB::Config cfg;

    cfg.dev_name        = ICHNAEA_DB_FLASH_DEV_NAME;
    cfg.part_name       = ICHNAEA_DB_PDI_RGN_NAME;
    cfg.ram_kvdb        = &s_kv_ram_db;
    cfg.dev_sector_size = HW::NOR::ERASE_BLOCK_SIZE;

    if( s_pdi_kvdb.configure( cfg ) != DB_ERR_NONE )
    {
      Panic::throwError( Panic::ErrorCode::ERR_SYSTEM_INIT_FAIL );
      return;
    }

    /*-------------------------------------------------------------------------
    Power on the KVDB driver. After this call the system should be ready.
    -------------------------------------------------------------------------*/
    if( !s_pdi_kvdb.init() )
    {
      Panic::throwError( Panic::ErrorCode::ERR_SYSTEM_INIT_FAIL );
      return;
    }

    s_db_ready = DRIVER_INITIALIZED_KEY;
  }


  mb::db::NvmKVDB &pdiDB()
  {
    return s_pdi_kvdb;
  }

  bool pdi_insert_and_create( mb::db::KVNode &node, void *dflt_data, const size_t size )
  {
    using namespace mb;
    using namespace mb::db;

    if( s_db_ready != DRIVER_INITIALIZED_KEY )
    {
      return false;
    }

    /*-------------------------------------------------------------------------
    Insert the node into the database
    -------------------------------------------------------------------------*/
    if( !s_pdi_kvdb.insert( node ) )
    {
      mbed_assert_continue_msg( false, "PDI key %d insert fail", node.hashKey );
      return false;
    }

    /*-------------------------------------------------------------------------
    Check if the key already exists in the database
    -------------------------------------------------------------------------*/
    if( s_pdi_kvdb.exists( node.hashKey ) )
    {
      return true;
    }

    /*-------------------------------------------------------------------------
    Write the default data to the database
    -------------------------------------------------------------------------*/
    LOG_DEBUG( "Creating new PDI key %d", node.hashKey );
    if( s_pdi_kvdb.write( node.hashKey, dflt_data, size ) != DB_ERR_NONE )
    {
      mbed_assert_continue_msg( false, "PDI key %d dflt write fail", node.hashKey );
      return false;
    }

    return true;
  }
}    // namespace System::Database
