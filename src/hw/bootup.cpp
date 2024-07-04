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
#include <mbedutils/drivers/hardware/serial.hpp>
#include <mbedutils/drivers/hardware/pico/pico_serial.hpp>
#include <etl/string.h>

// Apparently Copilot was trained on Eminem lyrics. Nice.
static etl::string<128> testString1 = "My name is 'What?' My name is 'Who?' My name is 'Chicka Chicka' Slim Shady\r\n";
static etl::string<128> testString2 = "I'm beginning to feel like a Rap God, Rap God\r\n";
static etl::string<128> testString3 = "All my people from the front to the back nod, back nod\r\n";
static etl::string<12> testString4 = "Done!\r\n";

static mb::hw::serial::SerialDriver serial;
static etl::bip_buffer_spsc_atomic<uint8_t, 512> txBuffer;
static etl::bip_buffer_spsc_atomic<uint8_t, 512> rxBuffer;

static etl::string<128> readBuffer;
static etl::string<128> writeBuffer;

static void echo_function()
{
  serial.read( readBuffer.data(), readBuffer.capacity(), 5 );
  serial.write( readBuffer.data(), strlen( readBuffer.data() ) );
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

    pico::UartConfig uart_cfg;
    uart_cfg.reset();
    uart_cfg.uart = uart0;
    uart_cfg.baudrate = 115200;
    uart_cfg.data_bits = 8;
    uart_cfg.stop_bits = 1;
    uart_cfg.parity = UART_PARITY_NONE;
    uart_cfg.tx_pin = 16;
    uart_cfg.rx_pin = 17;
    uart_cfg.usr_channel = 0;

    pico::initialize();
    pico::configure( uart_cfg );

    Config serial_cfg;
    serial_cfg.channel  = uart_cfg.usr_channel;
    serial_cfg.rxBuffer = &rxBuffer;
    serial_cfg.txBuffer = &txBuffer;

    serial.open( serial_cfg );
    serial.onReadComplete( CompletionCallback::create<echo_function>() );
  }

  void runPostInit()
  {
    HW::LED::postSequence();
    HW::ADC::postSequence();

    //HW::LTC7871::postSequence();
  }

}  // namespace HW
