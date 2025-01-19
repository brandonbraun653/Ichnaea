/******************************************************************************
 *  File Name:
 *    freertos_hooks.cpp
 *
 *  Description:
 *    FreeRTOS hook functions for the simulator
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdio>
#include <csignal>

extern "C" void vAssertCalled( const char *const pcFileName, unsigned long ulLine )
{
  ( void ) pcFileName;
  ( void ) ulLine;

  /* Break into the debugger */
  raise( SIGTRAP );

  while( 1 )
  {
    /* Do nothing */
  }
}

extern "C" void vApplicationDaemonTaskStartupHook( void )
{
  /* Do nothing */
}
