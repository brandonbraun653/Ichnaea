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
#include <src/threads/ichnaea_threads.hpp>
#include <mbedutils/threading.hpp>
#include <mbedutils/util.hpp>

namespace Threads
{
  using namespace mb::thread;

  /*---------------------------------------------------------------------------
  Local Enumerations
  ---------------------------------------------------------------------------*/

  /**
   * @brief System thread priorites, higher is more important.
   */
  enum ThreadPriority
  {
    PRIORITY_DELAYED_IO = 5,  /**< Always preemptible. Slow stuff here. */
    PRIORITY_BACKGROUND = 10, /**< Low priority background software */
    PRIORITY_CONTROL    = 15, /**< Control is fairly important */
    PRIORITY_MONITOR    = 20, /**< Monitor trumps all. System safety net. */
  };

  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static mb::thread::Internal::ControlBlockStorage<TSK_COUNT_MAX> s_tsk_cb;

  /* Background Thread */
  static Task                            s_background_task;
  static Task::Storage<4096, TaskMsg, 1> s_background_storage;

  /* Monitor Thread */
  static Task                            s_monitor_task;
  static Task::Storage<2048, TaskMsg, 1> s_monitor_storage;

  /* Control Thread */
  static Task                            s_control_task;
  static Task::Storage<4096, TaskMsg, 1> s_control_storage;

  /* Delayed I/O Thread */
  static Task                            s_delayed_io_task;
  static Task::Storage<4096, TaskMsg, 1> s_delayed_io_storage;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    /*-------------------------------------------------------------------------
    Initialize the threading module
    -------------------------------------------------------------------------*/
    mb::thread::Internal::ModuleConfig mod_cfg;
    mod_cfg.tsk_control_blocks = &s_tsk_cb;

    mb::thread::driver_setup( mod_cfg );

    /*-------------------------------------------------------------------------
    Add the background thread
    -------------------------------------------------------------------------*/
    s_background_storage.name = "Background";

    mb::thread::Task::Config cfg;
    cfg.reset();
    cfg.name                = s_background_storage.name;
    cfg.id                  = TSK_BACKGROUND_ID;
    cfg.func                = backgroundThread;
    cfg.affinity            = 0x3;
    cfg.priority            = PRIORITY_BACKGROUND;
    cfg.stack_buf           = s_background_storage.stack;
    cfg.stack_size          = count_of_array( s_background_storage.stack );
    cfg.msg_queue_cfg.pool  = &s_background_storage.msg_queue_storage.pool;
    cfg.msg_queue_cfg.queue = &s_background_storage.msg_queue_storage.queue;
    cfg.msg_queue_inst      = &s_background_storage.msg_queue;
    cfg.block_on_create     = false;    // Starting before the scheduler is running

    s_background_task = mb::thread::create( cfg );
    s_background_task.start();

    /*-------------------------------------------------------------------------
    Add the monitor thread
    -------------------------------------------------------------------------*/
    s_monitor_storage.name = "Monitor";

    cfg.reset();
    cfg.name                = s_monitor_storage.name;
    cfg.id                  = TSK_MONITOR_ID;
    cfg.func                = monitorThread;
    cfg.affinity            = 0x3;
    cfg.priority            = PRIORITY_MONITOR;
    cfg.stack_buf           = s_monitor_storage.stack;
    cfg.stack_size          = count_of_array( s_monitor_storage.stack );
    cfg.msg_queue_cfg.pool  = &s_monitor_storage.msg_queue_storage.pool;
    cfg.msg_queue_cfg.queue = &s_monitor_storage.msg_queue_storage.queue;
    cfg.msg_queue_inst      = &s_monitor_storage.msg_queue;
    cfg.block_on_create     = true;

    s_monitor_task = mb::thread::create( cfg );

    /*-------------------------------------------------------------------------
    Add the control thread
    -------------------------------------------------------------------------*/
    s_control_storage.name = "Control";

    cfg.reset();
    cfg.name                = s_control_storage.name;
    cfg.id                  = TSK_CONTROL_ID;
    cfg.func                = controlThread;
    cfg.affinity            = 0x3;
    cfg.priority            = PRIORITY_CONTROL;
    cfg.stack_buf           = s_control_storage.stack;
    cfg.stack_size          = count_of_array( s_control_storage.stack );
    cfg.msg_queue_cfg.pool  = &s_control_storage.msg_queue_storage.pool;
    cfg.msg_queue_cfg.queue = &s_control_storage.msg_queue_storage.queue;
    cfg.msg_queue_inst      = &s_control_storage.msg_queue;
    cfg.block_on_create     = true;

    s_control_task = mb::thread::create( cfg );

    /*-------------------------------------------------------------------------
    Add the delayed I/O thread
    -------------------------------------------------------------------------*/
    s_delayed_io_storage.name = "DelayedIO";

    cfg.reset();
    cfg.name                = s_delayed_io_storage.name;
    cfg.id                  = TSK_DELAYED_IO_ID;
    cfg.func                = delayedIOThread;
    cfg.affinity            = 0x3;
    cfg.priority            = PRIORITY_DELAYED_IO;
    cfg.stack_buf           = s_delayed_io_storage.stack;
    cfg.stack_size          = count_of_array( s_delayed_io_storage.stack );
    cfg.msg_queue_cfg.pool  = &s_delayed_io_storage.msg_queue_storage.pool;
    cfg.msg_queue_cfg.queue = &s_delayed_io_storage.msg_queue_storage.queue;
    cfg.msg_queue_inst      = &s_delayed_io_storage.msg_queue;
    cfg.block_on_create     = true;

    s_delayed_io_task = mb::thread::create( cfg );
  }


  void startThread( SystemTask task )
  {
    switch( task )
    {
      case TSK_BACKGROUND_ID:
        s_background_task.start();
        break;

      case TSK_MONITOR_ID:
        s_monitor_task.start();
        break;

      case TSK_CONTROL_ID:
        s_control_task.start();
        break;

      case TSK_DELAYED_IO_ID:
        s_delayed_io_task.start();
        break;

      default:
        break;
    }
  }


  void stopThread( SystemTask task )
  {
    switch( task )
    {
      case TSK_BACKGROUND_ID:
        s_background_task.kill();
        s_background_task.join();
        break;

      case TSK_MONITOR_ID:
        s_monitor_task.kill();
        s_monitor_task.join();
        break;

      case TSK_CONTROL_ID:
        s_control_task.kill();
        s_control_task.join();
        break;

      case TSK_DELAYED_IO_ID:
        s_delayed_io_task.kill();
        s_delayed_io_task.join();
        break;

      default:
        break;
    }
  }


  void join( SystemTask task )
  {
    switch( task )
    {
      case TSK_BACKGROUND_ID:
        s_background_task.join();
        break;

      case TSK_MONITOR_ID:
        s_monitor_task.join();
        break;

      case TSK_CONTROL_ID:
        s_control_task.join();
        break;

      case TSK_DELAYED_IO_ID:
        s_delayed_io_task.join();
        break;

      default:
        break;
    }
  }


  void sendSignal( const SystemTask task, const TaskMsgId id )
  {
    mb::thread::Message msg;
    Threads::TaskMsg    signal;

    signal.id = id;
    msg.data  = &signal;
    msg.size  = sizeof( signal );

    mb::thread::sendMessage( task, msg, mb::thread::TIMEOUT_BLOCK );
  }


  void sendMessage( const SystemTask task, TaskMsg &msg )
  {
    mb::thread::Message tsk_msg;
    tsk_msg.data = &msg;
    tsk_msg.size = sizeof( msg );

    mb::thread::sendMessage( task, tsk_msg, mb::thread::TIMEOUT_BLOCK );
  }
}    // namespace Threads
