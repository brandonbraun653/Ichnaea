/******************************************************************************
 *  File Name:
 *    nor.hpp
 *
 *  Description:
 *    NOR interface for supporting database storage
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_NOR_HPP
#define ICHNAEA_NOR_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/


namespace HW::NOR
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initialize the NOR driver
   */
  void initialize();
}  // namespace HW::NOR

#endif  /* !ICHNAEA_NOR_HPP */
