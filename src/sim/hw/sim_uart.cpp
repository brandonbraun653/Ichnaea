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
#include "mbedutils/assert.hpp"
#include "sim_serial.hpp"
#include <etl/bip_buffer_spsc_atomic.h>
#include <filesystem>
#include <src/bsp/board_map.hpp>
#include <src/hw/uart.hpp>
#include <src/sim/sim_ports.hpp>

namespace HW::UART
{
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
    using namespace mb::hw;

    /*-------------------------------------------------------------------------
    Construct the directory used for the IPC pipes
    -------------------------------------------------------------------------*/
    if( !std::filesystem::exists( SIM::kZMQ_EP_PATH ) )
    {
      mbed_assert_continue_msg( std::filesystem::create_directories( SIM::kZMQ_EP_PATH ),
                                "Failed to create directory for IPC pipes" );
    }

    /*-------------------------------------------------------------------------
    Configure the BMS UART channel
    -------------------------------------------------------------------------*/
    mb::hw::serial::sim::configure( UART_BMS, BSP::getZMQEndpoint( Peripheral::PERIPH_UART, UART_BMS ), true );

    Config serial_cfg;
    serial_cfg.channel  = HW::UART::UART_BMS;
    serial_cfg.rxBuffer = &s_bms_rx_buffer;
    serial_cfg.txBuffer = &s_bms_tx_buffer;

    mbed_assert( s_bms_driver.open( serial_cfg ) );

    /*-------------------------------------------------------------------------
    Configure the Debug UART channel
    -------------------------------------------------------------------------*/
    mb::hw::serial::sim::configure( UART_DEBUG, BSP::getZMQEndpoint( Peripheral::PERIPH_UART, UART_DEBUG ), true );

    serial_cfg.channel  = HW::UART::UART_DEBUG;
    serial_cfg.rxBuffer = &s_debug_rx_buffer;
    serial_cfg.txBuffer = &s_debug_tx_buffer;

    mbed_assert( s_debug_driver.open( serial_cfg ) );
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

}    // namespace HW::UART
