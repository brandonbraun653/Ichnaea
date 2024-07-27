/******************************************************************************
 *  File Name:
 *    system_util.hpp
 *
 *  Description:
 *    System utility functions and helpers
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_SYSTEM_UTILITY_HPP
#define ICHNAEA_SYSTEM_UTILITY_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>

namespace System
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Checks if the execution context is currently in an ISR
   *
   * @return true   The context is in an ISR
   * @return false  The context is not in an ISR
   */
  bool inISR();

  /**
   * @brief Unique identifier for the system.
   *
   * This value is generated from a 32-bit hash of the NOR flash device ID
   * attached to this processor. It should be unique across all Ichnaea systems.
   *
   * @return uint32_t
   */
  uint32_t identity();

}  // namespace System

#endif  /* !ICHNAEA_SYSTEM_UTILITY_HPP */
