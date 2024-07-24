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

namespace Threads
{
  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static constexpr size_t MonitorMsgDepth  = 15;
  static constexpr size_t MonitorStackSize = 1024;

  static mb::thread::Task                                                          s_monitor_task;
  static mb::thread::TaskConfigStorage<TaskMsg, MonitorMsgDepth, MonitorStackSize> s_monitor_storage;

  static mb::thread::TaskControlBlockStorage<5> s_tsk_cb;

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
    s_monitor_storage.name = "Monitor";

    mb::thread::TaskConfig cfg;
    cfg.name       = s_monitor_storage.name;
    cfg.id         = TSK_MONITOR_ID;
    cfg.func       = monitorThread;
    cfg.affinity   = 0x3;
    cfg.priority   = 15;
    cfg.stack_buf  = s_monitor_storage.stack;
    cfg.stack_size = count_of_array( s_monitor_storage.stack );
    cfg.msg_pool   = &s_monitor_storage.msg_pool;
    cfg.msg_queue  = &s_monitor_storage.msg_queue;

    s_monitor_task = mb::thread::create( cfg );

  }
}  // namespace Threads
