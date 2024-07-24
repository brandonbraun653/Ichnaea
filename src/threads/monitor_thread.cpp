/******************************************************************************
 *  File Name:
 *    monitor_thread.cpp
 *
 *  Description:
 *    Monitor thread to ensure that the system is operating correctly
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "src/app/app_decl.hpp"
#include "src/bsp/board_map.hpp"
#include "src/threads/ichnaea_threads.hpp"
#include "src/system/system_sensor.hpp"
#include "src/hw/led.hpp"
#include <mbedutils/logging.hpp>

#include <FreeRTOS.h>
#include <task.h>
namespace Threads
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void monitorThread( void *arg )
  {
    vTaskDelay( pdMS_TO_TICKS( 1500 ) );
    HW::LED::setBrightness( HW::LED::Channel::STATUS_0, 0.5f );
    while( 1 )
    {
      HW::LED::toggle( HW::LED::Channel::STATUS_0 );
      vTaskDelay( pdMS_TO_TICKS( 500 ) );
    }
  }
}    // namespace Threads
