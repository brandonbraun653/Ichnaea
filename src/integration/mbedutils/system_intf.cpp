/******************************************************************************
 *  File Name:
 *    system_intf.cpp
 *
 *  Description:
 *    Project specific implementation of the mbedutils system interface
 *
 *  2025 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/interfaces/system_intf.hpp>
#include <src/app/pdi/boot_count.hpp>

namespace mb::system::intf
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  size_t get_boot_count()
  {
    return App::PDI::getBootCount();
  }
}    // namespace mb::system::intf
