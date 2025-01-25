/******************************************************************************
 *  File Name:
 *    background_thread.cpp
 *
 *  Description:
 *    Background process for Idle things and low priority tasks
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "mbedutils/drivers/threading/thread.hpp"
#include <mbedutils/logging.hpp>
#include <src/app/app_config.hpp>
#include <src/app/app_filter.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/app_power.hpp>
#include <src/app/app_stats.hpp>
#include <src/app/pdi/boot_count.hpp>
#include <src/app/proto/ichnaea_async.pb.h>
#include <src/bsp/board_map.hpp>
#include <src/com/ctrl_server.hpp>
#include <src/hw/led.hpp>
#include <src/sim/sim_lifetime.hpp>
#include <src/system/system_bootup.hpp>
#include <src/system/system_shutdown.hpp>
#include <src/system/system_util.hpp>
#include <src/threads/ichnaea_threads.hpp>

namespace Threads
{
  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Emits a heartbeat message to the control server
   *
   * This is used to indicate that the system is still alive and well.
   */
  static void emitHeartbeat()
  {
    static size_t            last_invoked = 0;
    static ichnaea_Heartbeat signal       = ichnaea_Heartbeat_init_default;

    size_t current_time = mb::time::millis();
    if( current_time - last_invoked > 1000 )
    {
      last_invoked      = current_time;
      signal.boot_count = App::PDI::getBootCount();
      signal.node_id    = System::identity();
      signal.timestamp  = current_time;

      Control::getRPCServer().publishMessage( ichnaea_AsyncMessageId_MSG_HEARTBEAT, &signal );
    }
  }

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void backgroundThread( void *arg )
  {
    ( void )arg;

    /*-------------------------------------------------------------------------
    Finish the remainder of the bootup sequence
    -------------------------------------------------------------------------*/
    System::Boot::initTech();
    System::Boot::runPostInit();

    /*-------------------------------------------------------------------------
    Start powering on application level drivers
    -------------------------------------------------------------------------*/
    App::Config::driver_init();
    App::Stats::driver_init();
    App::Power::driver_init();
    App::Filter::driver_init();
    App::Monitor::driver_init();

    /*-------------------------------------------------------------------------
    Signal the next thread in the sequence to start. Simulators must wait b/c
    the above steps execute too fast. Other threads aren't ready to start.
    -------------------------------------------------------------------------*/
    mb::thread::this_thread::sleep_for( 5 );
    startThread( SystemTask::TSK_DELAYED_IO_ID );

    HW::LED::setBrightness( HW::LED::Channel::HEARTBEAT, 0.5f );

    /*-------------------------------------------------------------------------
    Run the background task
    -------------------------------------------------------------------------*/
    mb::thread::Message tsk_msg;
    Threads::TaskMsg    signal;

    tsk_msg.data = &signal;
    tsk_msg.size = sizeof( signal );

    while( !mb::thread::this_thread::task()->killPending() )
    {
      /*-----------------------------------------------------------------------
      Receive Messages
      -----------------------------------------------------------------------*/
      if( mb::thread::this_thread::awaitMessage( tsk_msg, 500 ) )
      {
        switch( signal.id )
        {
          case TSK_MSG_SHUTDOWN:
            System::Shutdown::initiate();
            break;

          default:
            break;
        }
      }

      /*-----------------------------------------------------------------------
      Run background tasks
      -----------------------------------------------------------------------*/
      emitHeartbeat();
      HW::LED::toggle( HW::LED::Channel::HEARTBEAT );
    }

    /*-------------------------------------------------------------------------
    Shutdown sequence
    -------------------------------------------------------------------------*/
    LOG_INFO( "Background thread shutting down" );

    // TODO: Reverse the bootup sequence here

    SIM::shutdown();

    /*-------------------------------------------------------------------------
    Finally, restart
    -------------------------------------------------------------------------*/
    System::Shutdown::Internal::trigger_reset();
  }
}    // namespace Threads
