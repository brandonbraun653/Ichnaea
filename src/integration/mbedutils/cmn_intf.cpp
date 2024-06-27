/******************************************************************************
 *  File Name:
 *    cmn_intf.cpp
 *
 *  Description:
 *    Ichnaea integration of the mbedutils common interface
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/interfaces/cmn_intf.hpp>

namespace mbedutils::intf
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  size_t max_drivers( const Driver driver )
  {
    return 0;
  }


  size_t max_driver_index( const Driver driver )
  {
    return 0;
  }


  bool is_driver_available( const Driver driver, const size_t channel )
  {
    return false;
  }

}  // namespace mbedutils::cmn
