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
#include <src/app/pdi/boot_count.hpp>
#include <src/system/system_db.hpp>

namespace App::Stats
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void driver_init()
  {
    /*-------------------------------------------------------------------------
    Register the PDI keys for the statistics tracking system
    -------------------------------------------------------------------------*/
    App::PDI::pdi_register_key_boot_count();

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
