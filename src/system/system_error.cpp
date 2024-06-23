/******************************************************************************
 *  File Name:
 *    system_error.cpp
 *
 *  Description:
 *    System error handler implementation
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "src/bsp/board_map.hpp"
#include "src/system/panic_handlers.hpp"
#include "src/system/system_error.hpp"
#include <cstddef>
#include <cstring>

namespace Panic
{
  /*---------------------------------------------------------------------------
  Private Data
  ---------------------------------------------------------------------------*/

  static volatile ErrorCode s_last_error;
  static ErrorCallback      s_error_handlers[ static_cast<size_t>( ErrorCode::NUM_OPTIONS ) ];

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void powerUp()
  {
    s_last_error = ErrorCode::NO_ERROR;
    memset( s_error_handlers, 0, sizeof( s_error_handlers ) );
  }


  bool throwError( const ErrorCode code )
  {
    s_last_error = code;

    /*-------------------------------------------------------------------------
    Break into the debugger if it's attached. It's likely we want to inspect
    the state of the system at this point.
    -------------------------------------------------------------------------*/
    #if defined( ICHNAEA_EMBEDDED )
    __asm volatile( "bkpt #0" );
    #endif  /* ICHNAEA_EMBEDDED */

    /*-------------------------------------------------------------------------
    Call the appropriate error handler
    -------------------------------------------------------------------------*/
    if( ( code < ErrorCode::NUM_OPTIONS ) && ( s_error_handlers[ static_cast<size_t>( code ) ] != nullptr ) )
    {
      return s_error_handlers[ static_cast<size_t>( code ) ]( code );
    }
    else
    {
      return Handlers::DefaultHandler( code );
    }
  }


  void assertion( const bool predicate, const ErrorCode code )
  {
    if( !predicate )
    {
      throwError( code );
    }
  }

  ErrorCode getLastError()
  {
    return s_last_error;
  }


  void resetError()
  {
    s_last_error = ErrorCode::NO_ERROR;
  }


  void registerHandler( const ErrorCode code, ErrorCallback callback )
  {
    if( code < ErrorCode::NUM_OPTIONS )
    {
      s_error_handlers[ static_cast<size_t>( code ) ] = callback;
    }
  }
}    // namespace Panic
