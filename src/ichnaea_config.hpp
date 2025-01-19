/******************************************************************************
 *  File Name:
 *    ichnaea_config.hpp
 *
 *  Description:
 *    Static configuration that applies to all of Ichnaea software
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_CONFIG_HPP
#define ICHNAEA_CONFIG_HPP

namespace Config
{
  /**
   * @brief Break into the debugger when a Panic::ErrorCode is thrown.
   *
   * This only applies for builds that are being debugged on either the
   * simulator or a physical target.
   */
  static constexpr bool DEBUG_BREAK_ON_PANIC = false;

}    // namespace Config

#endif /* !ICHNAEA_CONFIG_HPP */
