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
#include "etl/bip_buffer_spsc_atomic.h"
#include "src/bsp/board_map.hpp"
#include "src/hw/uart.hpp"

namespace HW::UART
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  static constexpr size_t DEFAULT_BAUD   = 9200;
  static constexpr size_t TX_BUFFER_SIZE = 1024;
  static constexpr size_t RX_BUFFER_SIZE = 512;

  /*---------------------------------------------------------------------------
  Aliases
  ---------------------------------------------------------------------------*/

  /**
   * @brief Alias the bipartite buffer type for uniform DMA buffer management.
   * This is assuming the buffer size is less than 65536 bytes.
   *
   * @tparam N Size of the buffer to create
   */
  template<const size_t N>
  using bip_buffer = etl::bip_buffer_spsc_atomic<uint8_t, N, etl::memory_model::MEMORY_MODEL_MEDIUM>;

  /*---------------------------------------------------------------------------
  Structures
  ---------------------------------------------------------------------------*/

  struct UARTControlBlock
  {
    critical_section_t         rtx_cs;   /**< IRQ exclusive access protection */
    mutex_t                    txlock;   /**< TX buffer protections */
    mutex_t                    rxlock;   /**< RX buffer protections */
    bip_buffer<TX_BUFFER_SIZE> txBuffer; /**< TX output buffer */
    bip_buffer<RX_BUFFER_SIZE> rxBuffer; /**< RX input buffer */
  };

  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static etl::array<UARTControlBlock, Channel::NUM_OPTIONS> s_uart_cb;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    /*-------------------------------------------------------------------------
    Initialize the UART pins
    -------------------------------------------------------------------------*/
  }


  void configure( const size_t channel, const uint32_t baudRate )
  {
  }


  bool acquire( const size_t channel, const size_t timeout )
  {
    return false;
  }


  void release( const size_t channel )
  {
  }


  size_t txAvailable( const size_t channel )
  {
    return 0;
  }


  size_t rxAvailable( const size_t channel )
  {
    return 0;
  }


  size_t write( const size_t channel, const uint8_t *const data, const size_t length )
  {
    return 0;
  }


  size_t read( const size_t channel, uint8_t *const data, const size_t length )
  {
    return 0;
  }

}  // namespace HW::UART
