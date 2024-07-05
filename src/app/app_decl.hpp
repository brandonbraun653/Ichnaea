/******************************************************************************
 *  File Name:
 *    app_decl.hpp
 *
 *  Description:
 *    Application function declarations
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_APP_DECLARATIONS_HPP
#define ICHNAEA_APP_DECLARATIONS_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/


namespace App
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Process measured system data
   *
   * This function is responsible for measuring the raw system data and
   * publishing it to the appropriate data structures for consumption by
   * the rest of the system.
   */
  void procSystemMeasurements();
}  // namespace App

#endif  /* !ICHNAEA_APP_DECLARATIONS_HPP */
