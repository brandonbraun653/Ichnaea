/******************************************************************************
 *  File Name:
 *    nor.cpp
 *
 *  Description:
 *    NOR interface for supporting database storage
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "mbedutils/drivers/logging/logging_macros.hpp"
#include <mbedutils/interfaces/gpio_intf.hpp>
#include <mbedutils/interfaces/spi_intf.hpp>
#include <mbedutils/memory.hpp>
#include <src/bsp/board_map.hpp>
#include <src/hw/nor.hpp>
#include <src/system/system_error.hpp>

/*-----------------------------------------------------------------------------
Module Literals
-----------------------------------------------------------------------------*/
#define NOR_DEBUG 0

namespace HW::NOR
{
  using namespace mb::memory;
  using namespace mb::memory::nor;

  /*---------------------------------------------------------------------------
  Private Data
  ---------------------------------------------------------------------------*/

  static DeviceDriver s_flash_mem;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  int init()
  {
    using namespace mb::hw;

    auto io_cfg = BSP::getIOConfig();

    /*-------------------------------------------------------------------------
    Initialize the GPIO pins for SPI control
    -------------------------------------------------------------------------*/
    /* SCK */
    gpio_init( io_cfg.spi[ BSP::SPI_NOR_FLASH ].sck );
    gpio_set_function( io_cfg.spi[ BSP::SPI_NOR_FLASH ].sck, GPIO_FUNC_SPI );
    gpio_pull_down( io_cfg.spi[ BSP::SPI_NOR_FLASH ].sck );

    /* MOSI */
    gpio_init( io_cfg.spi[ BSP::SPI_NOR_FLASH ].mosi );
    gpio_set_function( io_cfg.spi[ BSP::SPI_NOR_FLASH ].mosi, GPIO_FUNC_SPI );
    gpio_pull_down( io_cfg.spi[ BSP::SPI_NOR_FLASH ].mosi );

    /* MISO */
    gpio_init( io_cfg.spi[ BSP::SPI_NOR_FLASH ].miso );
    gpio_set_function( io_cfg.spi[ BSP::SPI_NOR_FLASH ].miso, GPIO_FUNC_SPI );
    gpio_pull_up( io_cfg.spi[ BSP::SPI_NOR_FLASH ].miso );

    /* CS */
    auto pin = BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_SPI_CS_NOR );
    gpio_init( pin );
    gpio_set_dir( pin, GPIO_OUT );
    gpio_pull_up( pin );
    gpio_put( pin, 1 );

    /*-------------------------------------------------------------------------
    Initialize the SPI peripheral
    -------------------------------------------------------------------------*/
    constexpr uint32_t spi_clk_rate = 31'250'000;

    auto pSPI = reinterpret_cast<spi_inst_t *>( BSP::getHardware( mb::hw::PERIPH_SPI, BSP::SPI_NOR_FLASH ) );

    const uint32_t act_baud = spi_init( pSPI, spi_clk_rate );
    if( ( act_baud < ( spi_clk_rate * 0.9f ) ) || ( act_baud > ( spi_clk_rate * 1.1f ) ) )
    {
      Panic::throwError( Panic::ErrorCode::ERR_SYSTEM_INIT_FAIL );
    }

    spi_set_format( pSPI, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST );

    /*-------------------------------------------------------------------------
    Configure the NOR flash driver
    -------------------------------------------------------------------------*/
    DeviceConfig cfg;

    cfg.dev_attr.size               = 4 * 1024 * 1024;
    cfg.dev_attr.block_size         = 4096;
    cfg.dev_attr.write_size         = 256;
    cfg.dev_attr.read_size          = 256;
    cfg.dev_attr.erase_size         = 4096;
    cfg.dev_attr.erase_chip_latency = 50'000;    // 50 seconds
    cfg.dev_attr.write_latency      = 3;
    cfg.dev_attr.erase_latency      = 400;
    cfg.spi_port                    = io_cfg.spi[ BSP::SPI_NOR_FLASH ].port;
    cfg.spi_cs_pin                  = io_cfg.gpio[ BSP::GPIO_SPI_CS_NOR ].pin;
    cfg.spi_cs_port                 = 0;    // This doesn't mean anything for the Pico implementation
    cfg.use_hs_read                 = true;
    cfg.pend_event_cb               = mb::memory::nor::device::adesto_at25sfxxx_pend_event;

    s_flash_mem.open( cfg );

    /*-------------------------------------------------------------------------
    Verify the driver is ready by getting the manufacturer information
    -------------------------------------------------------------------------*/
    auto info = s_flash_mem.getDeviceInfo();

    if( info.manufacturer == 0 || info.type == 0 || info.capacity == 0 )
    {
      Panic::throwError( Panic::ErrorCode::ERR_SYSTEM_INIT_FAIL );
      return -1;
    }

    return 0;
  }


  int read( long offset, uint8_t *buf, size_t size )
  {
    LOG_TRACE_IF( NOR_DEBUG, "Read 0x%08X, %d bytes", offset, size );
    return s_flash_mem.read( offset, buf, size ) == Status::ERR_OK ? size : -1;
  }


  int write( long offset, const uint8_t *buf, size_t size )
  {
    LOG_TRACE_IF( NOR_DEBUG, "Write 0x%08X, %d bytes", offset, size );
    int result = s_flash_mem.write( offset, buf, size ) == Status::ERR_OK ? size : -1;

/*-------------------------------------------------------------------------
Integrity check to make sure the data was written correctly
-------------------------------------------------------------------------*/
#if NOR_DEBUG
    static etl::array<uint8_t, 512> s_debug_buffer;

    if( result > 0 )
    {
      s_flash_mem.read( offset, s_debug_buffer.data(), size );
      if( memcmp( buf, s_debug_buffer.data(), size ) != 0 )
      {
        result = -1;
        LOG_ERROR( "Write 0x%08X miscompare", offset );
      }
    }
#endif

    return result;
  }


  int erase( long offset, size_t size )
  {
    return s_flash_mem.erase( offset, size ) == Status::ERR_OK ? size : -1;
  }


}    // namespace HW::NOR
