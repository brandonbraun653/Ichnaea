/******************************************************************************
 *  File Name:
 *    nor.cpp
 *
 *  Description:
 *    NOR interface for supporting database storage
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "mbedutils/drivers/memory/nvm/nor_flash.hpp"
#include <src/hw/nor.hpp>
#include <nor_flash_file.hpp>

namespace HW::NOR
{
  /*---------------------------------------------------------------------------
  Private Data
  ---------------------------------------------------------------------------*/

  static fake::memory::nor::FileFlash mNorFlash;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  int init()
  {
    mb::memory::nor::DeviceConfig flash_cfg;
    flash_cfg.dev_attr.block_size = ERASE_BLOCK_SIZE;
    flash_cfg.dev_attr.size = FLASH_ADDR_MAX;

    mNorFlash.open( "nor_flash.bin", flash_cfg );
    return 0;
  }


  int read( long offset, uint8_t* buf, size_t size )
  {
    return ( mb::memory::Status::ERR_OK == mNorFlash.read( offset, buf, size ) ) ? 0 : -1;
  }


  int write( long offset, const uint8_t* buf, size_t size )
  {
    return ( mb::memory::Status::ERR_OK == mNorFlash.write( offset, buf, size ) ) ? 0 : -1;
  }


  int erase( long offset, size_t size )
  {
    return ( mb::memory::Status::ERR_OK == mNorFlash.erase( offset, size ) ) ? 0 : -1;
  }

}  // namespace HW::NOR
