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
#include <src/system/system_config.hpp>
#include <src/hw/nor.hpp>
#include "fal_def.h"

namespace System::Config
{
  /*---------------------------------------------------------------------------
  Private Data
  ---------------------------------------------------------------------------*/

  /**
   * @brief FlashDB descriptor for the NOR flash device
   */
  static const fal_flash_dev s_fdb_nor_flash_descriptor = {
    .name       = "nor_flash_0",
    .addr       = HW::NOR::FLASH_ADDR_MIN,
    .len        = HW::NOR::FLASH_ADDR_MAX,
    .blk_size   = HW::NOR::ERASE_BLOCK_SIZE,
    .ops        = { .init = HW::NOR::init, .read = HW::NOR::read, .write = HW::NOR::write, .erase = HW::NOR::erase },
    .write_gran = 1
  };


  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {

  }
}  // namespace System::Config
