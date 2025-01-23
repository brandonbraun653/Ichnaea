/******************************************************************************
 *  File Name:
 *    delayed_io_thread.cpp
 *
 *  Description:
 *    Thread to handle delayed I/O operations
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/logging.hpp>
#include <mbedutils/threading.hpp>
#include <src/system/system_db.hpp>
#include <src/threads/ichnaea_threads.hpp>

namespace Threads
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void delayedIOThread( void *arg )
  {
    ( void )arg;

    mb::thread::Message tsk_msg;
    Threads::TaskMsg    signal;

    tsk_msg.data = &signal;
    tsk_msg.size = sizeof( signal );

    /*-------------------------------------------------------------------------
    Signal the next thread in the sequence to start
    -------------------------------------------------------------------------*/
    startThread( SystemTask::TSK_MONITOR_ID );

    /*-------------------------------------------------------------------------
    Run the task
    -------------------------------------------------------------------------*/
    while( !mb::thread::this_thread::task()->killPending() )
    {
      /*-----------------------------------------------------------------------
      Receive Messages
      -----------------------------------------------------------------------*/
      if( mb::thread::this_thread::awaitMessage( tsk_msg, 100 ) )
      {
        switch( signal.id )
        {
          case TSK_MSG_FLUSH_PDI:
            System::Database::pdiDB().flush();
            break;

          default:
            break;
        }
      }

      /*-----------------------------------------------------------------------
      Perform delayed I/O operations
      -----------------------------------------------------------------------*/
      System::Database::pdiDB().flush();
    }

    /*-------------------------------------------------------------------------
    Shutdown sequence
    -------------------------------------------------------------------------*/
    LOG_INFO( "Delayed I/O thread shutting down" );
  }
}    // namespace Threads
