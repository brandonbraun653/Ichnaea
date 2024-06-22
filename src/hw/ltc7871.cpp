/******************************************************************************
 *  File Name:
 *    ltc7871.cpp
 *
 *  Description:
 *    Ichnaea LTC7871 driver implementation. This driver assumes single core
 *    access with no concurrency protection. All operations are blocking.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "etl/array.h"
#include "src/bsp/board_map.hpp"
#include "src/hw/ltc7871.hpp"
#include "src/hw/ltc7871_reg.hpp"
#include "src/system/system_error.hpp"

/*-----------------------------------------------------------------------------
Local Literals
-----------------------------------------------------------------------------*/

#define SPI ( spi1 )

namespace HW::LTC7871
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  static constexpr size_t SPI_BAUD = 4'000'000; /**< Max clock for LTC7871 is 5MHz */
  static constexpr uint8_t PEC_INIT = 0x41; /**< Initial PEC value for LTC7871 */

  /*---------------------------------------------------------------------------
  Aliases
  ---------------------------------------------------------------------------*/

  using spi_txfr_buffer_t = etl::array<uint8_t, 3>;

  /*---------------------------------------------------------------------------
  Private Function Declarations
  ---------------------------------------------------------------------------*/

  static void    write_register( const uint8_t reg, const uint8_t data );
  static uint8_t read_register( const uint8_t reg );
  static uint8_t compute_pec( const spi_txfr_buffer_t &buffer );
  static bool    on_ltc_error( const Panic::ErrorCode &err );

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    auto config = BSP::getIOConfig();

    /*-------------------------------------------------------------------------
    Power up the GPIO control lines
    -------------------------------------------------------------------------*/
    /* Ensures the controller is off */
    gpio_init( config.gpio.ltcRun );
    gpio_set_dir( config.gpio.ltcRun, GPIO_OUT );
    gpio_put( config.gpio.ltcRun, 0 );

    /* Set the mode control to Burst */
    gpio_init( config.gpio.ltcCcm );
    gpio_set_dir( config.gpio.ltcCcm, GPIO_OUT );
    gpio_put( config.gpio.ltcCcm, 0 );

    gpio_init( config.gpio.ltcDcm );
    gpio_set_dir( config.gpio.ltcDcm, GPIO_OUT );
    gpio_put( config.gpio.ltcDcm, 0 );

    /* Use the FREQ pin resistor to set initial switching frequency */
    gpio_init( config.pwm.ltcSync );
    gpio_set_dir( config.pwm.ltcSync, GPIO_OUT );
    gpio_put( config.pwm.ltcSync, 1 );

    /*-------------------------------------------------------------------------
    Power up the SPI control lines
    -------------------------------------------------------------------------*/
    gpio_init( config.spi.sck );
    gpio_set_function( config.spi.sck, GPIO_FUNC_SPI );
    gpio_pull_down( config.spi.sck );

    gpio_init( config.spi.mosi );
    gpio_set_function( config.spi.mosi, GPIO_FUNC_SPI );
    gpio_pull_down( config.spi.mosi );

    gpio_init( config.spi.miso );
    gpio_set_function( config.spi.miso, GPIO_FUNC_SPI );
    gpio_pull_down( config.spi.miso );

    gpio_init( config.gpio.spiCs0 );
    gpio_set_dir( config.gpio.spiCs0, GPIO_OUT );
    gpio_pull_up( config.gpio.spiCs0 );
    gpio_put( config.gpio.spiCs0, 1 );

    /*-------------------------------------------------------------------------
    Initialize the SPI peripheral
    -------------------------------------------------------------------------*/
    const uint act_baud = spi_init( SPI, SPI_BAUD );
    if( act_baud > SPI_BAUD )
    {
      Panic::throwError( Panic::ErrorCode::SYSTEM_INIT_FAIL );
    }

    spi_set_format( SPI, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST );

    /*-------------------------------------------------------------------------
    Map error handlers for the LTC7871 driver
    -------------------------------------------------------------------------*/
    Panic::registerHandler( Panic::ErrorCode::LTC7871_PEC_READ_FAIL, on_ltc_error );
    Panic::registerHandler( Panic::ErrorCode::LTC7871_DATA_READ_FAIL, on_ltc_error );
    Panic::registerHandler( Panic::ErrorCode::LTC7871_PEC_WRITE_FAIL, on_ltc_error );
    Panic::registerHandler( Panic::ErrorCode::LTC7871_DATA_WRITE_FAIL, on_ltc_error );
  }


  void postSequence()
  {
    // TODO: validate the CONFIG1/2 reigster settings to inspect hw state on boot.
  }


  float getAverageCurrent()
  {
    return 0.0f;
  }


  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Write a register on the LTC7871 with the given data.
   *
   * This function ensures that either the data is fully committed without
   * error, or the LTC7871 is placed into a fault state and the system is
   * shutdown.
   *
   * @param reg Register address to write to
   * @param data Data to write to the register
   */
  static void write_register( const uint8_t reg, const uint8_t data )
  {
    /*-------------------------------------------------------------------------
    Compute the buffer to send over SPI
    -------------------------------------------------------------------------*/
    spi_txfr_buffer_t tx_buf = { static_cast<uint8_t>( ( reg << 1u ) & 0xFF ), data, PEC_INIT };
    tx_buf[ 2 ] = compute_pec( tx_buf );

    /*-------------------------------------------------------------------------
    Perform the SPI transfer
    -------------------------------------------------------------------------*/
    gpio_put( BSP::getIOConfig().gpio.spiCs0, 0 );
    const int write_size = spi_write_blocking( SPI, tx_buf.data(), tx_buf.size() );
    gpio_put( BSP::getIOConfig().gpio.spiCs0, 1 );

    if( write_size != static_cast<int>( tx_buf.size() ) )
    {
      Panic::throwError( Panic::ErrorCode::LTC7871_DATA_WRITE_FAIL );
    }

    /*-------------------------------------------------------------------------
    Ensure there was no errors applying the transaction
    -------------------------------------------------------------------------*/
    const auto ctrl_reg = read_register( REG_MFR_CHIP_CTRL );
    if( ( ctrl_reg & MFR_CHIP_CTRL_CML_Msk ) == MFR_CHIP_CTRL_CML_Fault )
    {
      Panic::throwError( Panic::ErrorCode::LTC7871_PEC_WRITE_FAIL );
    }

    const auto applied_data = read_register( reg );
    if( applied_data != data )
    {
      Panic::throwError( Panic::ErrorCode::LTC7871_DATA_WRITE_FAIL );
    }
  }


  /**
   * @brief Reads a register from the LTC7871.
   *
   * This function ensures that the data read is valid by checking the PEC code
   * that is returned with the data. Should an error occur, the LTC7871 is placed
   * into a fault state and the system is shutdown.
   *
   * @param reg Register address to read from
   * @return uint8_t Data read from the register
   */
  static uint8_t read_register( const uint8_t reg )
  {
    /*-------------------------------------------------------------------------
    Prepare data for the read
    -------------------------------------------------------------------------*/
    const uint8_t read_addr = ( reg << 1u ) | 1u;
    spi_txfr_buffer_t rx_buf = {};

    /*-------------------------------------------------------------------------
    Perform the SPI transfer
    -------------------------------------------------------------------------*/
    gpio_put( BSP::getIOConfig().gpio.spiCs0, 0 );
    const int read_size = spi_read_blocking( SPI, read_addr, rx_buf.data(), rx_buf.size() );
    gpio_put( BSP::getIOConfig().gpio.spiCs0, 1 );

    if ( read_size != static_cast<int>( rx_buf.size() ) )
    {
      Panic::throwError( Panic::ErrorCode::LTC7871_DATA_READ_FAIL );
    }

    /*-------------------------------------------------------------------------
    Validate the PEC code, calculated over the address we sent + the returned
    data byte.
    -------------------------------------------------------------------------*/
    rx_buf[ 0 ] = read_addr;
    const uint8_t pec = compute_pec( rx_buf );

    if( pec != rx_buf[ 2 ] )
    {
      Panic::throwError( Panic::ErrorCode::LTC7871_PEC_READ_FAIL );
    }

    return rx_buf[ 1 ];
  }


  /**
   * @brief Computes the PEC code for the given buffer
   *
   * @param buffer Buffer to compute the PEC code for
   * @return uint8_t PEC code
   */
  static uint8_t compute_pec( const spi_txfr_buffer_t &buffer )
  {
    // Datasheet states the PEC is calculated over address and data bytes
    uint16_t data = static_cast<uint16_t>( buffer[ 0 ] << 8u | buffer[ 1 ] );
    uint16_t pec  = PEC_INIT;

    // Iterate from MSB first
    for( int i = 15; i >= 0; i-- )
    {
      uint8_t din = ( data >> i ) & 1u;              // Get the current data bit
      uint8_t in0 = din ^ ( ( pec >> 7u ) & 1u );    // XOR the highest PEC bit with the current data bit
      uint8_t in1 = in0 ^ ( ( pec >> 0u ) & 1u );    // XOR lowest PEC bit with the result of the previous XOR
      uint8_t in2 = in0 ^ ( ( pec >> 1u ) & 1u );    // XOR the second lowest PEC bit with the result of the first XOR

      // Shift PEC left by 1 and update lowest bits
      pec = ( pec << 1u ) & 0xF8;
      pec |= ( in2 | in1 | in0 ) & 0x07;
    }

    return pec;
  }


  /**
   * @brief Error handler for PEC calculation failures
   *
   * @param err Error code that was thrown
   * @return bool
   */
  static bool on_ltc_error( const Panic::ErrorCode &err )
  {
    // SHIT. Need to handle this. It's basically a critical shutdown event. We cannot
    // ever lose control of the registers.
    return true;
  }
}    // namespace HW::LTC7871
