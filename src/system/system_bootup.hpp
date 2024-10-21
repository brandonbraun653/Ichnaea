/******************************************************************************
 *  File Name:
 *    system_bootup.hpp
 *
 *  Description:
 *    Initialization routines for the hardware on bootup
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_SYSTEM_BOOTUP_HPP
#define ICHNAEA_SYSTEM_BOOTUP_HPP

namespace System::Boot
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initialize hardware drivers for the system.
   *
   * These are core peripheral initialization sequences that must be run to
   * configure the hardware for execution. This includes things like GPIO,
   * SPI, I2C, etc.
   */
  void initDrivers();

  /**
   * @brief Initialize high level technology stack drivers.
   *
   * These components depend on the system runtime to be intialized and running
   * before they can be set up. This includes things like logging, databases,
   * and other HW based components.
   */
  void initTech();

  /**
   * @brief Performs the Power On Self Test (POST) for the system
   */
  void runPostInit();

}  // namespace System::Boot

#endif  /* !ICHNAEA_SYSTEM_BOOTUP_HPP */
