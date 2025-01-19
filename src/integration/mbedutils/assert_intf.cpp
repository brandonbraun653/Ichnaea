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
#include <src/bsp/board_map.hpp>
#include <mbedutils/interfaces/assert_intf.hpp>
#include <mbedutils/interfaces/system_intf.hpp>
#include <mbedutils/logging.hpp>
#include <mbedutils/util.hpp>

namespace mb::assert
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void on_assert_fail( const bool halt, const etl::string_view &msg )
  {
    /*-------------------------------------------------------------------------
    Log the error message to the debug output
    -------------------------------------------------------------------------*/
    logging::log( logging::Level::LVL_ERROR, msg.data(), msg.size() );

    /*-------------------------------------------------------------------------
    Halt the system if requested
    -------------------------------------------------------------------------*/
    if( halt )
    {
      mb::util::breakpoint();
      mb::system::intf::warm_reset();
    }
  }

}    // namespace mb::assert
