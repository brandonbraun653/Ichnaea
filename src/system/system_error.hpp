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
  enum class ErrorCode : uint32_t
  {
    NO_ERROR = 0,
    UNKNOWN,
    BOARD_VERSION_READ_FAIL,
    SYSTEM_THREAD_EXIT,

    NUM_OPTIONS
  };

  /*---------------------------------------------------------------------------
  Aliases
  ---------------------------------------------------------------------------*/

  using ErrorCallback = bool ( * )( const ErrorCode& );

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
  bool throwSystemError( const ErrorCode code );

  /**
   * @brief Registers a handler for a specific error code
   *
   * @param code    Error code to register the handler for
   * @param handler Function to call when the error occurs
   */
  void registerHandler( const ErrorCode code, ErrorCallback handler );

}  // namespace Panic

#endif  /* !ICHNAEA_SYSTEM_ERROR_HPP */
