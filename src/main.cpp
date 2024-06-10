/******************************************************************************
 *  File Name:
 *    main.cpp
 *
 *  Description:
 *    Main entry point for Ichnaea
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "src/bsp/board_map.hpp"
#include "src/system/system_error.hpp"

/*-----------------------------------------------------------------------------
Public Functions
-----------------------------------------------------------------------------*/

/**
 * @brief Core entry point for Ichnaea
 *
 * This is responsible for initializing the hardware and starting the main
 * monitor and control system threads.
 *
 * @return int
 */
int main()
{
  Panic::powerUp();
  BSP::powerUp();

  /*---------------------------------------------------------------------------
  Initialize the GPIO pin
  ---------------------------------------------------------------------------*/
  const uint LED_PIN = 20;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  while( 1 )
  {
    gpio_put( LED_PIN, 1 );
    sleep_ms( 500 );
    gpio_put( LED_PIN, 0 );
    sleep_ms( 500 );
  }

  /*---------------------------------------------------------------------------
  Main thread should never return
  ---------------------------------------------------------------------------*/
  return -1;
}

/* NOTES:
- Monitor thread
- Control thread
- Spawn the HW init sequences first. Might need to register. Start monitor thread.
Then have the monitor start the control.
*/