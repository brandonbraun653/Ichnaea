/******************************************************************************
 *  File Name:
 *    system_control.hpp
 *
 *  Description:
 *    Interface for high level system control behaviors
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_SYSTEM_CONTROL_HPP
#define ICHNAEA_SYSTEM_CONTROL_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>

namespace System
{
  /*---------------------------------------------------------------------------
  Enumerations
  ---------------------------------------------------------------------------*/

  enum class ResetType : uint8_t
  {
    REBOOT,
    BOOTLOADER,

    NUM_OPTIONS
  };

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Resets the system to a known state
   *
   * @param target  Target state to reset to
   */
  void reset( const ResetType target );

}  // namespace System

#endif  /* !ICHNAEA_SYSTEM_CONTROL_HPP */
