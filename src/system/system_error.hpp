/******************************************************************************
 *  File Name:
 *    system_error.hpp
 *
 *  Description:
 *    Interface for handling system level errors
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_SYSTEM_ERROR_HPP
#define ICHNAEA_SYSTEM_ERROR_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>
#include <cstddef>

/*-----------------------------------------------------------------------------
Macros
-----------------------------------------------------------------------------*/

#define PANIC_HANDLER_DECL( NAME ) bool NAME( const ErrorCode &code )
#define PANIC_HANDLER_DEF( NAME ) bool NAME( const ErrorCode &code )

namespace Panic
{
  /*---------------------------------------------------------------------------
  Enumerations
  ---------------------------------------------------------------------------*/

  /**
   * @brief Error codes that can be reported by the system
   */
  enum class ErrorCode : size_t
  {
    NO_ERROR = 0,
    UNKNOWN,
    ASSERTION_FAIL,
    INVALID_PARAM,
    INVALID_CONTEXT,
    SYSTEM_INIT_FAIL,
    POST_FAIL,
    BOARD_VERSION_READ_FAIL,
    SYSTEM_THREAD_EXIT,
    LTC7871_DATA_WRITE_FAIL,
    LTC7871_DATA_READ_FAIL,
    LTC7871_PEC_READ_FAIL,
    LTC7871_PEC_WRITE_FAIL,

    NUM_OPTIONS
  };

  /*---------------------------------------------------------------------------
  Aliases
  ---------------------------------------------------------------------------*/

  using ErrorCallback = bool ( * )( const ::Panic::ErrorCode & );

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initialize the panic handler systems
   */
  void powerUp();

  /**
   * @brief Handles a system level error by entering a panic state
   *
   * @param code  Error code to report
   * @return bool True if the handler was able to recover, false otherwise
   */
  bool throwError( const ErrorCode code );

  /**
   * @brief Get the last thrown error code
   * @return ErrorCode
   */
  ErrorCode getLastError();

  /**
   * @brief Resets the error code back to NO_ERROR
   */
  void resetError();

  /**
   * @brief Registers a handler for a specific error code
   *
   * @param code    Error code to register the handler for
   * @param handler Function to call when the error occurs
   */
  void registerHandler( const ErrorCode code, ErrorCallback handler );

}    // namespace Panic

#endif /* !ICHNAEA_SYSTEM_ERROR_HPP */
