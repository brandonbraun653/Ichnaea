/******************************************************************************
 *  File Name:
 *    assert_intf.cpp
 *
 *  Description:
 *    Ichnaea integration of the mb assert interface
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/interfaces/assert_intf.hpp>

namespace mb::assert
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void on_assert_fail( const bool halt, const etl::string_view &msg )
  {
    if( halt )
    {
      __asm volatile( "bkpt #0" );
    }
  }

}  // namespace mb::assert
