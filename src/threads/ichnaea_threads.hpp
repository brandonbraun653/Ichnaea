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
#include <cstdint>
#include <mbedutils/thread.hpp>

namespace Threads
{
  /*---------------------------------------------------------------------------
  Enumerations
  ---------------------------------------------------------------------------*/

  enum SystemTask : mb::thread::TaskId
  {
    TSK_BACKGROUND_ID = 0,
    TSK_MONITOR_ID,
    TSK_CONTROL_ID,

    TSK_COUNT_MAX
  };

  /*---------------------------------------------------------------------------
  Structures
  ---------------------------------------------------------------------------*/

  /**
   * @brief Task message type for Ichnaea
   */
  struct TaskMsg
  {
    mb::thread::TaskMsgId id;
    union _MsgType
    {
      // Temporary for now
      float    a;
      uint32_t b;
      uint8_t  c;
    } msg;
  };

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initialize the project threading system.
   */
  void initialize();

  /**
   * @brief Low priority background thread to handle non-critical tasks.
   *
   * @param arg Unused
   */
  void backgroundThread( void* arg );

  /**
   * @brief Monitors the system for proper operation and reports any issues.
   *
   * This is tasked with monitoring system health, such as temperature, voltage,
   * current, etc. It will also be responsible for reporting any issues to the
   * control thread.
   *
   * @param arg Unused
   */
  void monitorThread( void* arg );

  /**
   * @brief A high priority low latency thread to directly control the system.
   *
   * This is tasked with implementing MPPT algorithms, emergency shutdowns,
   * communication with the BMS, etc.
   *
   * @param arg Unused
   */
  void controlThread( void* arg );

}    // namespace Threads

#endif /* !ICHNAEA_THREADS_HPP */
