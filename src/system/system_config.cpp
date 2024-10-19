/******************************************************************************
 *  File Name:
 *    system_config.cpp
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
#include <mbedutils/util.hpp>
#include <src/integration/flashdb/fal_cfg.h>
#include <src/system/system_config.hpp>
#include <src/system/system_error.hpp>
#include <src/hw/nor.hpp>
#include "fal_def.h"

namespace System::Config
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  static constexpr size_t GAIN_MAX_COUNT      = 100;
  static constexpr size_t GAIN_TRANSCODE_SIZE = 512;

  /*---------------------------------------------------------------------------
  Public Data
  ---------------------------------------------------------------------------*/

  /**
   * @brief FlashDB descriptor for the NOR flash device
   */
  extern "C" const struct fal_flash_dev fdb_nor_flash0 = {
    .name       = "nor_flash_0",
    .addr       = HW::NOR::FLASH_ADDR_MIN,
    .len        = HW::NOR::FLASH_ADDR_MAX,
    .blk_size   = HW::NOR::ERASE_BLOCK_SIZE,
    .ops        = { .init = HW::NOR::init, .read = HW::NOR::read, .write = HW::NOR::write, .erase = HW::NOR::erase },
    .write_gran = 1
  };


  /*---------------------------------------------------------------------------
  Private Data
  ---------------------------------------------------------------------------*/

  static size_t s_db_ready;
  static mb::db::NvmKVDB s_kvdb;
  static mb::db::NvmKVDB::Storage<GAIN_MAX_COUNT, GAIN_TRANSCODE_SIZE> s_kvdb_storage;


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
    ram_cfg.node_storage     = &s_kvdb_storage.kv_nodes;
    ram_cfg.transcode_buffer = s_kvdb_storage.transcode_buffer;

    if( s_kvdb_storage.kv_ram_db.configure( ram_cfg ) != DB_ERR_NONE )
    {
      Panic::throwError( Panic::ErrorCode::ERR_SYSTEM_INIT_FAIL );
      return;
    }

    /*-------------------------------------------------------------------------
    Configure the NVM based KVDB driver
    -------------------------------------------------------------------------*/
    NvmKVDB::Config cfg;

    cfg.dev_name        = ICHNAEA_DB_FLASH_DEV_NAME;
    cfg.part_name       = ICHNAEA_DB_GAIN_RGN_NAME;
    cfg.ram_kvdb        = &s_kvdb_storage.kv_ram_db;
    cfg.dev_sector_size = HW::NOR::ERASE_BLOCK_SIZE;

    if( s_kvdb.configure( cfg ) != DB_ERR_NONE )
    {
      Panic::throwError( Panic::ErrorCode::ERR_SYSTEM_INIT_FAIL );
      return;
    }

    /*-------------------------------------------------------------------------
    Power on the KVDB driver. After this call the system should be ready.
    -------------------------------------------------------------------------*/
    if( !s_kvdb.init() )
    {
      Panic::throwError( Panic::ErrorCode::ERR_SYSTEM_INIT_FAIL );
      return;
    }

    s_db_ready = DRIVER_INITIALIZED_KEY;
  }
}  // namespace System::Config
