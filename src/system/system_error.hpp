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
  enum ErrorCode : size_t
  {
    /*-------------------------------------------------------------------------
    General Errors
    -------------------------------------------------------------------------*/
    NO_ERROR = 0,
    ERR_UNKNOWN,                 /* An unknown error occurred */
    ERR_ASSERTION_FAIL,          /* An assertion of some kind failed */
    ERR_INVALID_PARAM,           /* Bad argument inputs to a function */
    ERR_INVALID_CONTEXT,         /* Something ran where it wasn't supposed to */
    ERR_SYSTEM_INIT_FAIL,        /* Failed to initialize a driver */
    ERR_POST_FAIL,               /* Driver POST sequence failed */
    ERR_BOARD_VERSION_READ_FAIL, /* Failed to determine HW revision */
    ERR_SYSTEM_THREAD_EXIT,      /* System thread terminated when it should not have */

    /*-------------------------------------------------------------------------
    LTC7871 Errors
    -------------------------------------------------------------------------*/
    _ERR_LTC_START,
    ERR_LTC_DATA_WRITE_FAIL = _ERR_LTC_START, /* Unable to write data to SPI */
    ERR_LTC_DATA_READ_FAIL,                   /* Unable to read data from SPI */
    ERR_LTC_PEC_READ_FAIL,                    /* Ichnaea computed PEC miscompare for the PEC sent by LTC */
    ERR_LTC_PEC_WRITE_FAIL,                   /* LTC computed PEC miscompare for the PEC sent by Ichnaea */
    ERR_LTC_CMD_FAIL,                         /* A commanded interaction failed to apply as expected */
    ERR_LTC_HW_STRAP_FAIL,                    /* The hardware strap configuration is invalid */
    ERR_LTC_FAULT,                            /* The LTC7871 has thrown a fault code */
    _ERR_LTC_END,

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
   * @brief Throws an error if the predicate is false
   *
   * @param predicate Condition to evaluate
   * @param code    Error code to throw if the predicate is false
   */
  void assertion( const bool predicate, const ErrorCode code );

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
