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

//! TESTING
#include <mbedutils/interfaces/serial_intf.hpp>
#include <mbedutils/drivers/hardware/pico/pico_serial.hpp>
#include <etl/string.h>

// Apparently Copilot was trained on Eminem lyrics. Nice.
static etl::string<128> testString  = "My name is 'What?' My name is 'Who?' My name is 'Chicka Chicka' Slim Shady\r\n";
static etl::string<128> testString2 = "I'm beginning to feel like a Rap God, Rap God\r\n";
static etl::string<128> testString3 = "All my people from the front to the back nod, back nod\r\n";

static void test_callback( const size_t channel, const size_t num_bytes)
{
  using namespace mb::hw::serial;

  if( num_bytes == testString.size() )
  {
    intf::write_async( channel, testString2.c_str(), testString2.size() );
  }
  else if( num_bytes == testString2.size() )
  {
    intf::write_async( channel, testString3.c_str(), testString3.size() );
  }
  else if( num_bytes == testString3.size() )
  {
    intf::write_async( channel, testString.c_str(), testString.size() );
  }

}

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


    // !TESTING
    using namespace mb::hw::serial;

    pico::UartConfig config;
    config.reset();
    config.uart = uart0;
    config.baudrate = 115200;
    config.data_bits = 8;
    config.stop_bits = 1;
    config.parity = UART_PARITY_NONE;
    config.tx_pin = 16;
    config.rx_pin = 17;
    config.usr_channel = 0;

    pico::initialize();
    pico::configure( config );
    intf::lock( 0, 1000 );

    intf::on_tx_complete( config.usr_channel, intf::TXCompleteCallback::create<test_callback>() );

    intf::write_async( 0, testString.c_str(), testString.size() );
  }

  void runPostInit()
  {
    HW::LED::postSequence();
    HW::ADC::postSequence();

    //HW::LTC7871::postSequence();
  }

}  // namespace HW
