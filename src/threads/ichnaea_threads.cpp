/******************************************************************************
 *  File Name:
 *    ichnaea_threads.cpp
 *
 *  Description:
 *    Ichnaea system thread control
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>
#include <src/threads/ichnaea_threads.hpp>
#include <mbedutils/thread.hpp>
#include <mbedutils/util.hpp>

namespace mb::thread::intf
{
  inline constexpr size_t num_cores()
  {
    return 2;
  }

  inline constexpr size_t max_tasks()
  {
    return 5;
  }
}

namespace Threads
{
  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static constexpr size_t MonitorMsgDepth  = 15;
  static constexpr size_t MonitorStackSize = 1024;

  static mb::thread::Task                                                          s_monitor_task;
  static mb::thread::TaskConfigStorage<TaskMsg, MonitorMsgDepth, MonitorStackSize> s_monitor_storage;

  static mb::thread::TaskCBStorage<5> s_tsk_cb;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    /*-------------------------------------------------------------------------
    Initialize the threading module
    -------------------------------------------------------------------------*/
    mb::thread::ModuleConfig mod_cfg;
    mod_cfg.tsk_control_blocks = &s_tsk_cb;

    mb::thread::initialize( mod_cfg );

    /*-------------------------------------------------------------------------
    Add the monitor thread
    -------------------------------------------------------------------------*/
    mb::thread::TaskConfig cfg;
    cfg.name       = s_monitor_storage.name;
    cfg.func       = mb::thread::TaskFunction::create<monitorThread>();
    cfg.affinity   = 0;
    cfg.priority   = 128;
    cfg.stack_buf  = s_monitor_storage.stack;
    cfg.stack_size = count_of_array( s_monitor_storage.stack );
    cfg.msg_pool   = &s_monitor_storage.msg_pool;
    cfg.msg_queue  = &s_monitor_storage.msg_queue;

    s_monitor_task = mb::thread::create( cfg );

  }
}  // namespace Threads