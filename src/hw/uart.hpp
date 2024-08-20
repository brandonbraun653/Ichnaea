/******************************************************************************
 *  File Name:
 *    uart.hpp
 *
 *  Description:
 *    Ichnaea UART driver interface. This implements a multi-channel, thread
 *    safe, serial stream building block that can be used for building more
 *    complex communication protocols. The backend is implemented using DMA,
 *    so it should be pretty efficient.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_HW_UART_HPP
#define ICHNAEA_HW_UART_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstddef>
#include <cstdint>
#include <mbedutils/drivers/hardware/serial.hpp>

namespace HW::UART
{
  /*---------------------------------------------------------------------------
  Enumerations
  ---------------------------------------------------------------------------*/

  enum Channel : size_t
  {
    UART_BMS, /**< Battery management system interface */

    NUM_OPTIONS
  };

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initalize UART driver and hardware
   */
  void initialize();

  mb::hw::serial::SerialDriver &getDriver( const Channel channel );

}  // namespace HW::UART

#endif  /* !ICHNAEA_HW_UART_HPP */
