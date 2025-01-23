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
#include <mbedutils/threading.hpp>

namespace Threads
{
  /*---------------------------------------------------------------------------
  Enumerations
  ---------------------------------------------------------------------------*/

  enum SystemTask : mb::thread::TaskId
  {
    TSK_BACKGROUND_ID,
    TSK_MONITOR_ID,
    TSK_CONTROL_ID,
    TSK_DELAYED_IO_ID,

    TSK_COUNT_MAX
  };

  enum TaskMsgId : mb::thread::MessageId
  {
    TSK_MSG_SHUTDOWN,
    TSK_MSG_FLUSH_PDI,

    TSK_MSG_COUNT
  };

  /*---------------------------------------------------------------------------
  Structures
  ---------------------------------------------------------------------------*/

  /**
   * @brief Task message type for Ichnaea
   */
  struct TaskMsg
  {
    TaskMsgId id;

    union _MsgType
    {
      // Temporary for now
      float    a;
      uint32_t b;
      uint8_t  c;
    } data;
  };

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initialize the project threading system.
   */
  void initialize();

  /**
   * @brief Start execution of a system task
   *
   * @param task Which task to begin executing
   */
  void startThread( SystemTask task );

  /**
   * @brief Stops execution of a system task.
   *
   * Once called, the task cannot be started again until the system is reset.
   * This functionality is intended to support system shutdown procedures.
   *
   * @param task Which task to stop executing
   */
  void stopThread( SystemTask task );

  /**
   * @brief Waits for a system task to finish executing.
   *
   * @param task Which task to wait for
   */
  void join( SystemTask task );

  /**
   * @brief Sends a signal to a system task.
   *
   * @param task Which task to send the signal to
   * @param id   The message ID to send
   */
  void sendSignal( const SystemTask task, const TaskMsgId id );

  /**
   * @brief Sends a message to a system task.
   *
   * @param task Which task to send the message to
   * @param msg  The message to send
   */
  void sendMessage( const SystemTask, TaskMsg &msg );

  /**
   * @brief Low priority background thread to handle non-critical tasks.
   *
   * @param arg Unused
   */
  void backgroundThread( void *arg );

  /**
   * @brief Monitors the system for proper operation and reports any issues.
   *
   * This is tasked with monitoring system health, such as temperature, voltage,
   * current, etc. It will also be responsible for reporting any issues to the
   * control thread.
   *
   * @param arg Unused
   */
  void monitorThread( void *arg );

  /**
   * @brief A high priority low latency thread to directly control the system.
   *
   * This is tasked with implementing MPPT algorithms, emergency shutdowns,
   * communication with the BMS, etc.
   *
   * @param arg Unused
   */
  void controlThread( void *arg );

  /**
   * @brief A thread to handle delayed I/O operations.
   *
   * @param arg Unused
   */
  void delayedIOThread( void *arg );
}    // namespace Threads

#endif /* !ICHNAEA_THREADS_HPP */
