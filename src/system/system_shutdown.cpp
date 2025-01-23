/******************************************************************************
 *  File Name:
 *    system_shutdown.cpp
 *
 *  Description:
 *    Ichnaea shutdown procedures
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/logging.hpp>
#include <mbedutils/threading.hpp>
#include <mbedutils/system.hpp>
#include <src/bsp/board_map.hpp>
#include <src/system/system_shutdown.hpp>
#include <src/threads/ichnaea_threads.hpp>

namespace System::Shutdown
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initiate()
  {
    using namespace Threads;

    /*-------------------------------------------------------------------------
    Only perform the teardown from the background thread
    -------------------------------------------------------------------------*/
    auto current_task = mb::thread::this_thread::id();
    if( current_task != TSK_BACKGROUND_ID )
    {
      LOG_DEBUG( "Signaling background thread to shutdown" );
      Threads::sendSignal( SystemTask::TSK_BACKGROUND_ID, Threads::TSK_MSG_SHUTDOWN );
      return;
    }

    /*-------------------------------------------------------------------------
    Tear down all threads except the background task (current context). There's
    additional logic in the background task to handle the final shutdown.
    -------------------------------------------------------------------------*/
    LOG_INFO( "Shutdown procedure invoked" );
    stopThread( SystemTask::TSK_CONTROL_ID );
    stopThread( SystemTask::TSK_MONITOR_ID );
    stopThread( SystemTask::TSK_DELAYED_IO_ID );

    // Only publish the kill request. Don't join yet.
    mb::thread::this_thread::task()->kill();
  }


  void Internal::trigger_reset()
  {
    mb::system::intf::warm_reset();
  }


  void Internal::wait_for_system_teardown()
  {
    /*-------------------------------------------------------------------------
    As part of the tear down sequence, the background thread will be the last
    to exit. This function will block until that happens.
    -------------------------------------------------------------------------*/
    Threads::join( Threads::SystemTask::TSK_BACKGROUND_ID );
  }
}    // namespace System::Shutdown
