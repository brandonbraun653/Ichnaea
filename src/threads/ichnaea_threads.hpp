/******************************************************************************
 *  File Name:
 *    ichnaea_threads.hpp
 *
 *  Description:
 *    Thread declarations for the Ichnaea system
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_THREADS_HPP
#define ICHNAEA_THREADS_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/


namespace Threads
{
  /**
   * @brief Monitors the system for proper operation and reports any issues.
   *
   * This is tasked with monitoring system health, such as temperature, voltage,
   * current, etc. It will also be responsible for reporting any issues to the
   * control thread.
   */
  void monitorThread( void );

  /**
   * @brief A high priority low latency thread to directly control the system.
   *
   * This is tasked with implementing MPPT algorithms, emergency shutdowns,
   * communication with the BMS, etc.
   */
  void controlThread( void );

  // TODO: Need a message publishing interface similar to TSK_msg.
}    // namespace Threads

#endif /* !ICHNAEA_THREADS_HPP */
