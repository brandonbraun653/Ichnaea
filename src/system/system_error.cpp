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


  etl::string_view getErrorString( const Panic::ErrorCode code )
  {
    switch( code )
    {
      case NO_ERROR:
        return "No error";

      case ERR_UNKNOWN:
        return "Unknown error";

      case ERR_ASSERTION_FAIL:
        return "Assertion failed";

      case ERR_INVALID_PARAM:
        return "Invalid parameter";

      case ERR_INVALID_CONTEXT:
        return "Invalid context";

      case ERR_SYSTEM_INIT_FAIL:
        return "System initialization failed";

      case ERR_POST_FAIL:
        return "POST sequence failed";

      case ERR_BOARD_VERSION_READ_FAIL:
        return "Board version read failed";

      case ERR_SYSTEM_THREAD_EXIT:
        return "System thread exited unexpectedly";

      case ERR_LTC_DATA_WRITE_FAIL:
        return "LTC data write failed";

      case ERR_LTC_DATA_READ_FAIL:
        return "LTC data read failed";

      case ERR_LTC_PEC_READ_FAIL:
        return "LTC PEC read failed";

      case ERR_LTC_PEC_WRITE_FAIL:
        return "LTC PEC write failed";

      case ERR_LTC_CMD_FAIL:
        return "LTC command failed";

      case ERR_LTC_HW_STRAP_FAIL:
        return "LTC hardware strap failed";

      case ERR_LTC_FAULT:
        return "LTC fault";

      case ERR_LTC_PWR_DWN_FAIL:
        return "LTC power down failed";

      case ERR_MONITOR_12V0_OOR:
        return "12V rail out of range";

      case ERR_MONITOR_VIN_OOR:
        return "Input voltage out of range";

      case ERR_MONITOR_VOUT_OOR:
        return "Output voltage out of range";

      case ERR_MONITOR_IOUT_OOR:
        return "Output current out of range";

      case ERR_MONITOR_TEMP_OOR:
        return "Temperature out of range";

      case ERR_MONITOR_FAN_SPEED_OOR:
        return "Fan speed out of range";

      default:
        return "Unknown error code";
    }
  }

}    // namespace Panic
