/******************************************************************************
 *  File Name:
 *    app_stats.hpp
 *
 *  Description:
 *    Interface for storing and retrieving application statistics
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_APP_STATS_HPP
#define ICHNAEA_APP_STATS_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/


namespace App::Stats
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initialize the statistics tracking system
   */
  void driver_init();

}  // namespace App::Stats

#endif  /* !ICHNAEA_APP_STATS_HPP */
