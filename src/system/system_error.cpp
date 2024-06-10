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
#include "src/system/panic_handlers.hpp"
#include "src/system/system_error.hpp"
#include <cstddef>
#include <cstring>

namespace Panic
{
  /*---------------------------------------------------------------------------
  Private Data
  ---------------------------------------------------------------------------*/

  static ErrorCallback s_error_handlers[ static_cast<size_t>( ErrorCode::NUM_OPTIONS ) ];

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void powerUp()
  {
    memset( s_error_handlers, 0, sizeof( s_error_handlers ) );
  }


  bool throwSystemError( const ErrorCode code )
  {
    if( ( code < ErrorCode::NUM_OPTIONS ) && ( s_error_handlers[ static_cast<size_t>( code ) ] != nullptr ) )
    {
      return s_error_handlers[ static_cast<size_t>( code ) ]( code );
    }
    else
    {
      return Handlers::DefaultHandler( code );
    }
  }


  void registerHandler( const ErrorCode code, ErrorCallback callback )
  {
    if( code < ErrorCode::NUM_OPTIONS )
    {
      s_error_handlers[ static_cast<size_t>( code ) ] = callback;
    }
  }
}    // namespace Panic
