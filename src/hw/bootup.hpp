/******************************************************************************
 *  File Name:
 *    bootup.hpp
 *
 *  Description:
 *    Initialization routines for the hardware on bootup
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_HW_BOOTUP_HPP
#define ICHNAEA_HW_BOOTUP_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/


namespace HW
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initialize hardware drivers for the system
   */
  void initDrivers();

  /**
   * @brief Performs the Power On Self Test (POST) for the hardware
   */
  void runPostInit();

}  // namespace HW

#endif  /* !ICHNAEA_HW_BOOTUP_HPP */
