/******************************************************************************
 *  File Name:
 *    nor.hpp
 *
 *  Description:
 *    NOR interface for supporting FlashDB database storage
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_NOR_HPP
#define ICHNAEA_NOR_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>
#include <cstddef>

namespace HW::NOR
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  static constexpr size_t ERASE_BLOCK_SIZE = 4096;
  static constexpr size_t FLASH_ADDR_MIN   = 0x00000000;
  static constexpr size_t FLASH_ADDR_MAX   = 8 * 1024 * 1024;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initialize the NOR driver (FDBStub)
   */
  int init();

  /**
   * @brief Read data from the NOR flash
   *
   * @param offset    Address to start reading from
   * @param buf       Buffer to read data into
   * @param size      Number of bytes to read
   * @return int      0 on success, error code otherwise
   */
  int read( long offset, uint8_t* buf, size_t size );

  /**
   * @brief Write data to the NOR flash
   *
   * @param offset    Address to start writing to
   * @param buf       Buffer to write data from
   * @param size      Number of bytes to write
   * @return int      0 on success, error code otherwise
   */
  int write( long offset, const uint8_t* buf, size_t size );

  /**
   * @brief Erase a block of data in the NOR flash
   *
   * @param offset    Address to start erasing from
   * @param size      Number of bytes to erase
   * @return int      0 on success, error code otherwise
   */
  int erase( long offset, size_t size );

}  // namespace HW::NOR

#endif  /* !ICHNAEA_NOR_HPP */
