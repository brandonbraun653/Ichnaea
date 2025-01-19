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
#include <cstddef>
#include <etl/delegate.h>

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
    ERR_LTC_PWR_DWN_FAIL,                     /* Failed to power down the LTC7871. Likely HW failure. */
    _ERR_LTC_END,

    /*-------------------------------------------------------------------------
    Monitoring Errors
    -------------------------------------------------------------------------*/
    _ERR_MONITOR_START,

    ERR_MONITOR_12V0_OOR = _ERR_MONITOR_START, /* 12V rail entered invalid range */
    ERR_MONITOR_VIN_OOR,                       /* Input voltage entered invalid range */
    ERR_MONITOR_VOUT_OOR,                      /* Output voltage entered invalid range */
    ERR_MONITOR_IOUT_OOR,                      /* Output current entered invalid range */
    ERR_MONITOR_TEMP_OOR,                      /* Temperature entered invalid range */
    ERR_MONITOR_FAN_SPEED_OOR,                 /* Fan speed entered invalid range */
    _ERR_MONITOR_END,

    NUM_OPTIONS
  };

  /*---------------------------------------------------------------------------
  Aliases
  ---------------------------------------------------------------------------*/

  using ErrorCallback = etl::delegate<bool( const ::Panic::ErrorCode & )>;

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
  bool throwError( const Panic::ErrorCode code );

  /**
   * @brief Throws an error if the predicate is false
   *
   * @param predicate Condition to evaluate
   * @param code    Error code to throw if the predicate is false
   */
  void assertion( const bool predicate, const Panic::ErrorCode code );

  /**
   * @brief Get the last thrown error code
   * @return ErrorCode
   */
  Panic::ErrorCode getLastError();

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
  void registerHandler( const Panic::ErrorCode code, Panic::ErrorCallback handler );

}    // namespace Panic

#endif /* !ICHNAEA_SYSTEM_ERROR_HPP */
