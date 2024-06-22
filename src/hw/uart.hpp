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

  /**
   * @brief Configure a channel for a specific baud rate.
   *
   * This project assumes a typical 8N1 setup for all UART channels with
   * no hardware flow control. Just need to configure the baud rate.
   *
   * @param channel   Which channel to configure
   * @param baudRate  The baud rate to configure the channel to
   */
  void configure( const size_t channel, const uint32_t baudRate );

  /**
   * @brief Acquire the UART channel for exclusive use.
   *
   * @param channel Which channel to acquire
   * @param timeout How long to wait for the channel to become available (ms)
   * @return true   Successfully acquired the channel
   * @return false  Failed to acquire the channel
   */
  bool acquire( const size_t channel, const size_t timeout );

  /**
   * @brief Release the UART channel for other tasks to use
   *
   * @param channel Which channel to release
   */
  void release( const size_t channel );

  /**
   * @brief Checks how many bytes can be written to the UART channel
   *
   * @param channel Which channel to check
   * @return size_t Number of bytes that can be written
   */
  size_t txAvailable( const size_t channel );

  /**
   * @brief Checks how many bytes are available to read from the UART channel
   *
   * @param channel Which channel to check
   * @return size_t Number of bytes that can be read
   */
  size_t rxAvailable( const size_t channel );

  /**
   * @brief Writes a block of data to the UART channel
   *
   * This will queue internally if the UART is busy transmitting data.
   *
   * @param data   Pointer to the data to write
   * @param length Number of bytes to write
   * @return size_t  Number of bytes successfully written
   */
  size_t write( const size_t channel, const uint8_t *const data, const size_t length );

  /**
   * @brief Reads a block of data from the UART channel
   *
   * @param data Output buffer to store the read data
   * @param length Number of bytes to read
   * @return size_t  Number of bytes successfully read
   */
  size_t read( const size_t channel, uint8_t *const data, const size_t length );

}  // namespace HW::UART

#endif  /* !ICHNAEA_HW_UART_HPP */
