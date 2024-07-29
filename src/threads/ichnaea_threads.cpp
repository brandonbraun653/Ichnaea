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
  using namespace mb::thread;

  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static mb::thread::TaskControlBlockStorage<TSK_COUNT_MAX> s_tsk_cb;

  /* Background Thread */
  static Task                                s_background_task;
  static TaskConfigStorage<TaskMsg, 1, 2048> s_background_storage;

  /* Monitor Thread */
  static Task                                s_monitor_task;
  static TaskConfigStorage<TaskMsg, 1, 2048> s_monitor_storage;

  /* Control Thread */
  static Task                                s_control_task;
  static TaskConfigStorage<TaskMsg, 1, 2048> s_control_storage;


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
    Add the background thread
    -------------------------------------------------------------------------*/
    s_background_storage.name = "Background";

    mb::thread::TaskConfig cfg;
    cfg.reset();
    cfg.name       = s_background_storage.name;
    cfg.id         = TSK_BACKGROUND_ID;
    cfg.func       = backgroundThread;
    cfg.affinity   = 0x3;
    cfg.priority   = 10;
    cfg.stack_buf  = s_background_storage.stack;
    cfg.stack_size = count_of_array( s_background_storage.stack );
    cfg.msg_pool   = &s_background_storage.msg_pool;
    cfg.msg_queue  = &s_background_storage.msg_queue;

    s_background_task = mb::thread::create( cfg );

    /*-------------------------------------------------------------------------
    Add the monitor thread
    -------------------------------------------------------------------------*/
    s_monitor_storage.name = "Monitor";

    cfg.reset();
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

    /*-------------------------------------------------------------------------
    Add the control thread
    -------------------------------------------------------------------------*/
    s_control_storage.name = "Control";

    cfg.reset();
    cfg.name       = s_control_storage.name;
    cfg.id         = TSK_CONTROL_ID;
    cfg.func       = controlThread;
    cfg.affinity   = 0x3;
    cfg.priority   = 20;
    cfg.stack_buf  = s_control_storage.stack;
    cfg.stack_size = count_of_array( s_control_storage.stack );
    cfg.msg_pool   = &s_control_storage.msg_pool;
    cfg.msg_queue  = &s_control_storage.msg_queue;

    s_control_task = mb::thread::create( cfg );
  }
}  // namespace Threads
