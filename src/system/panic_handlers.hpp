/******************************************************************************
 *  File Name:
 *    panic_handlers.hpp
 *
 *  Description:
 *    Declaration of available system panic handlers
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_PANIC_HANDLERS_HPP
#define ICHNAEA_PANIC_HANDLERS_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <src/system/system_error.hpp>

namespace Panic::Handlers
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Standard error handler for when no other handler is available.
   * @warning This function will reset the system.
   *
   * @param code  Error code to report
   * @return bool True if the handler was able to recover, false otherwise
   */
  bool DefaultHandler( const Panic::ErrorCode &code );

  /**
   * @brief This handler does absolutely nothing.
   */
  bool NoopHandler( const Panic::ErrorCode &code );

  /**
   * @brief Safe the system if the board version cannot be determined.
   *
   * This is a critical error and the board should not be allowed to start
   * operating if the IO version cannot be determined.
   */
  bool FailToReadBoardVersion( const Panic::ErrorCode &code );
}    // namespace Panic::Handlers

#endif /* !ICHNAEA_PANIC_HANDLERS_HPP */
