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
#include <mbedutils/memory.hpp>
#include <src/bsp/board_map.hpp>
#include <src/hw/nor.hpp>
#include <src/system/system_error.hpp>

namespace HW::NOR
{
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
    constexpr uint32_t spi_clk_rate = 32'000'000;

    auto pSPI = reinterpret_cast<spi_inst_t *>( BSP::getHardware( mb::hw::PERIPH_SPI, BSP::SPI_NOR_FLASH ) );

    const uint32_t act_baud = spi_init( pSPI, spi_clk_rate );
    if( act_baud > spi_clk_rate )
    {
      Panic::throwError( Panic::ErrorCode::ERR_SYSTEM_INIT_FAIL );
    }

    spi_set_format( pSPI, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST );

    /*-------------------------------------------------------------------------
    Configure the NOR flash driver
    -------------------------------------------------------------------------*/


    DeviceConfig cfg;

    cfg.spi_port = BSP::SPI_NOR_FLASH;
    cfg.spi_cs_pin = BSP::GPIO_SPI_CS_NOR;
    cfg.spi_cs_port = 0; // This doesn't mean anything for the Pico implementation

    return -1;
  }


  int read( long offset, uint8_t* buf, size_t size )
  {
    return -1;
  }


  int write( long offset, const uint8_t* buf, size_t size )
  {
    return -1;
  }


  int erase( long offset, size_t size )
  {
    return -1;
  }


}  // namespace HW::NOR
