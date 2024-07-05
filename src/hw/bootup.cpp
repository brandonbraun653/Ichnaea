/******************************************************************************
 *  File Name:
 *    bootup.cpp
 *
 *  Description:
 *    Bootup routines for the hardware on startup
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "src/bsp/board_map.hpp"
#include "src/hw/adc.hpp"
#include "src/hw/bootup.hpp"
#include "src/hw/gpio.hpp"
#include "src/hw/led.hpp"
#include "src/hw/ltc7871.hpp"
#include "src/hw/uart.hpp"
#include "src/system/system_error.hpp"
#include "src/system/system_logging.hpp"
#include <mbedutils/assert.hpp>
#include <mbedutils/osal.hpp>
#include <mbedutils/logging.hpp>

namespace HW
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/
  void initDrivers()
  {
    timer_hw->dbgpause = 0;    // Do not pause the timer during debugging

    /*-------------------------------------------------------------------------
    Load system dependencies for the hardware (order matters here)
    -------------------------------------------------------------------------*/
    mb::osal::initOSALDrivers();
    mb::assert::initialize();
    Panic::powerUp();
    BSP::powerUp();

    /*-------------------------------------------------------------------------
    Initialize the hardware peripherals. Ordered by least complex/dependent
    to most complex/dependent.
    -------------------------------------------------------------------------*/
    HW::GPIO::initialize(); /* Must be first to init IO to a safe state */
    HW::LED::initialize();
    HW::ADC::initialize();
    HW::UART::initialize();

    HW::LTC7871::initialize();

    /*-------------------------------------------------------------------------
    Initialize system level modules that depend on the hardware
    -------------------------------------------------------------------------*/
    Logging::initialize();

    LOG_TRACE( "Hardware initialization complete" );
  }

  void runPostInit()
  {
    LOG_TRACE( "Running POST sequence" );
    HW::LED::postSequence();
    HW::ADC::postSequence();

    //HW::LTC7871::postSequence();
    LOG_TRACE( "POST sequence complete" );
  }

}  // namespace HW
