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
#include <mbedutils/interfaces/gpio_intf.hpp>
#include <mbedutils/interfaces/spi_intf.hpp>
#include <mbedutils/memory.hpp>
#include <src/bsp/board_map.hpp>
#include <src/hw/nor.hpp>
#include <src/system/system_error.hpp>

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
    gpio::PinConfig pin_cfg;

    /* SCK */
    pin_cfg.pin       = io_cfg.spi[ BSP::SPI_NOR_FLASH ].sck;
    pin_cfg.drive     = GPIO_DRIVE_STRENGTH_12MA;
    pin_cfg.mode      = gpio::Mode_t::MODE_OUTPUT;
    pin_cfg.speed     = GPIO_SLEW_RATE_FAST;
    pin_cfg.pull      = gpio::Pull_t::PULL_DOWN;
    pin_cfg.alternate = GPIO_FUNC_SPI;

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
    constexpr uint32_t spi_clk_rate = 1'000'000;

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


  int read( long offset, uint8_t* buf, size_t size )
  {
    return s_flash_mem.read( offset, buf, size ) == Status::ERR_OK ? size : -1;
  }


  int write( long offset, const uint8_t* buf, size_t size )
  {
    return s_flash_mem.write( offset, buf, size ) == Status::ERR_OK ? size : -1;
  }


  int erase( long offset, size_t size )
  {
    return s_flash_mem.erase( offset, size ) == Status::ERR_OK ? size : -1;
  }


}  // namespace HW::NOR
