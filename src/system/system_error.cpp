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
#include <cstddef>
#include <cstring>
#include <mbedutils/util.hpp>
#include <src/bsp/board_map.hpp>
#include <src/ichnaea_config.hpp>
#include <src/system/panic_handlers.hpp>
#include <src/system/system_error.hpp>

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
    for( size_t x = 0; x < static_cast<size_t>( ErrorCode::NUM_OPTIONS ); x++ )
    {
      s_error_handlers[ x ] = {};
    }
  }


  bool throwError( const ErrorCode code )
  {
    s_last_error = code;

    /*-------------------------------------------------------------------------
    Trap the system if configured and an error occurs
    -------------------------------------------------------------------------*/
    if constexpr( Config::DEBUG_BREAK_ON_PANIC )
    {
      mb::util::breakpoint();
    }

    /*-------------------------------------------------------------------------
    Call the appropriate error handler
    -------------------------------------------------------------------------*/
    if( ( code < ErrorCode::NUM_OPTIONS ) && ( s_error_handlers[ static_cast<size_t>( code ) ] ) )
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
    /*-------------------------------------------------------------------------
    If the predicate is false, throw the error, which should attempt to recover
    from the error condition. If the error handler fails to recover, the system
    will reset.
    -------------------------------------------------------------------------*/
    if( !predicate )
    {
      mbed_assert( throwError( code ) );
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
