/******************************************************************************
 *  File Name:
 *    system_shutdown.hpp
 *
 *  Description:
 *    Ichnaea shutdown procedures
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_SHUTDOWN_HPP
#define ICHNAEA_SHUTDOWN_HPP

namespace System::Shutdown
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Starts the shutdown sequence for the system.
   *
   * May be called from any contex and will trigger the background thread to
   * begin the shutdown process. This function will return immediately.
   */
  void initiate();

  namespace Internal
  {
    /**
    * @brief Immediately restarts the system.
    *
    * Do not call explicitly. This is intended to be called by the background
    * thread when the system is shutting down.
    */
    void trigger_reset();

    /**
     * @brief Waits for the system to finish shutting down.
     *
     * This function will block the calling thread until the system has fully
     * shutdown. It is intended to be called by the main thread after the
     * initiate() function has been called.
     */
    void wait_for_system_teardown();
  }
}  // namespace System

#endif  /* !ICHNAEA_SHUTDOWN_HPP */
