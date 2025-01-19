/******************************************************************************
 *  File Name:
 *    uart.cpp
 *
 *  Description:
 *    Ichnaea UART driver implementation
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <src/bsp/board_map.hpp>
#include <src/hw/uart.hpp>
#include <etl/bip_buffer_spsc_atomic.h>
#include <mbedutils/drivers/hardware/pico/pico_serial.hpp>

namespace HW::UART
{
  static_assert( ( size_t )Channel::NUM_OPTIONS == ( size_t )BSP::UART_MAX_PORTS, "UART channel mismatch" );

  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static mb::hw::serial::SerialDriver              s_bms_driver;
  static etl::bip_buffer_spsc_atomic<uint8_t, 512> s_bms_tx_buffer;
  static etl::bip_buffer_spsc_atomic<uint8_t, 512> s_bms_rx_buffer;

  static mb::hw::serial::SerialDriver              s_debug_driver;
  static etl::bip_buffer_spsc_atomic<uint8_t, 512> s_debug_tx_buffer;
  static etl::bip_buffer_spsc_atomic<uint8_t, 512> s_debug_rx_buffer;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    using namespace mb::hw::serial;

    /*-------------------------------------------------------------------------
    Initialize the Pico serial driver this module is built on top of
    -------------------------------------------------------------------------*/
    pico::initialize();

    /*-------------------------------------------------------------------------
    Configure the BMS UART channel
    -------------------------------------------------------------------------*/
    pico::UartConfig uart_cfg;
    auto io_cfg = BSP::getIOConfig();

    uart_cfg.reset();
    uart_cfg.uart        = io_cfg.uart[ UART_BMS ].pHw;
    uart_cfg.baudrate    = 115200;
    uart_cfg.data_bits   = 8;
    uart_cfg.stop_bits   = 1;
    uart_cfg.parity      = UART_PARITY_NONE;
    uart_cfg.tx_pin      = io_cfg.uart[ UART_BMS ].tx;
    uart_cfg.rx_pin      = io_cfg.uart[ UART_BMS ].rx;
    uart_cfg.usr_channel = UART_BMS;

    pico::configure( uart_cfg );

    Config serial_cfg;
    serial_cfg.channel  = HW::UART::UART_BMS;
    serial_cfg.rxBuffer = &s_bms_rx_buffer;
    serial_cfg.txBuffer = &s_bms_tx_buffer;

    mbed_assert( s_bms_driver.open( serial_cfg ) );

    /*-------------------------------------------------------------------------
    Configure the Debug UART channel
    -------------------------------------------------------------------------*/
    if( BSP::getBoardRevision() >= 2 )
    {
      uart_cfg.reset();
      uart_cfg.uart        = io_cfg.uart[ UART_DEBUG ].pHw;
      uart_cfg.baudrate    = 115200;
      uart_cfg.data_bits   = 8;
      uart_cfg.stop_bits   = 1;
      uart_cfg.parity      = UART_PARITY_NONE;
      uart_cfg.tx_pin      = io_cfg.uart[ UART_DEBUG ].tx;
      uart_cfg.rx_pin      = io_cfg.uart[ UART_DEBUG ].rx;
      uart_cfg.usr_channel = UART_DEBUG;

      pico::configure( uart_cfg );

      serial_cfg.channel = HW::UART::UART_DEBUG;
      serial_cfg.rxBuffer = &s_debug_rx_buffer;
      serial_cfg.txBuffer = &s_debug_tx_buffer;

      mbed_assert( s_debug_driver.open( serial_cfg ) );
    }
  }


  mb::hw::serial::SerialDriver &getDriver( const Channel channel )
  {
    switch( channel )
    {
      case UART_BMS:
        return s_bms_driver;

      case UART_DEBUG:
        return s_debug_driver;

      default:
        break;
    }

    mbed_assert( false );
    return s_bms_driver;
  }

}  // namespace HW::UART
