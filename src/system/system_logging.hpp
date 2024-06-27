/******************************************************************************
 *  File Name:
 *    system_logging.hpp
 *
 *  Description:
 *    Core logging interface for the Ichanea system. Most of the logging effort
 *    is being offloaded to the mbedutls library.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_SYSTEM_LOGGING_HPP
#define ICHNAEA_SYSTEM_LOGGING_HPP

namespace Logging
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Init all the logging interfaces used in the project
   */
  void initialize();

}  // namespace

#endif  /* !ICHNAEA_SYSTEM_LOGGING_HPP */
