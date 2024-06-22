/******************************************************************************
 *  File Name:
 *    ltc7871.hpp
 *
 *  Description:
 *    Ichnaea LTC7871 driver interface
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_LTC7871_HPP
#define ICHNAEA_LTC7871_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>
#include <cstddef>

namespace HW::LTC7871
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Init the driver and ensure the LTC7871 is in a safed state.
   */
  void initialize();

  /**
   * @brief Perform power on self test (POST) for the LTC7871
   */
  void postSequence();

  /**
   * @brief Read the current output of the LTC7871
   * @return float The current in Amps
   */
  float getAverageCurrent();

}  // namespace HW::LTC7871

#endif  /* !ICHNAEA_LTC7871_HPP */
