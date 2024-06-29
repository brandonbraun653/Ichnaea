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
#include "src/hw/ltc7871_prv.hpp"
#include "src/hw/ltc7871_reg.hpp"
#include "src/system/system_error.hpp"

namespace HW::LTC7871
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  static constexpr size_t SPI_BAUD     = 1'000'000; /* Max clock for LTC7871 is 5MHz */
  static constexpr size_t LTC_ADDR_IDX = 0;         /* Buffer index for register addresses */
  static constexpr size_t LTC_DATA_IDX = 1;         /* Buffer index for data field */
  static constexpr size_t LTC_PEC_IDX  = 2;         /* Buffer index for error check code */

  /*---------------------------------------------------------------------------
  Aliases
  ---------------------------------------------------------------------------*/

  using spi_txfr_buffer_t = etl::array<uint8_t, 3>;

  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static const BSP::IOConfig *s_io_config;
  static DriverMode           s_driver_mode;
  static FaultCode            s_fault_code;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    s_io_config   = &BSP::getIOConfig();
    s_driver_mode = DriverMode::DISABLED;
    s_fault_code  = FaultCode::NO_FAULT;

    /*-------------------------------------------------------------------------
    Power up the GPIO control lines
    -------------------------------------------------------------------------*/
    /* Ensures the controller is off */
    // TODO BMB: This needs to be swapped over to an output for mosfet control in V2.
    uint pin = BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_LTC_RUN );
    gpio_init( pin );
    gpio_set_dir( pin, GPIO_IN );
    gpio_pull_up( pin );

    /* Set the mode control to Burst */
    pin = BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_LTC_CCM );
    gpio_init( pin );
    gpio_set_dir( pin, GPIO_OUT );
    gpio_put( pin, 0 );

    pin = BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_LTC_DCM );
    gpio_init( pin );
    gpio_set_dir( pin, GPIO_OUT );
    gpio_put( pin, 0 );

    /* Use the FREQ pin resistor to set initial switching frequency */
    pin = BSP::getPin( mb::hw::PERIPH_PWM, BSP::PWM_LTC_SYNC );
    gpio_init( pin );
    gpio_set_dir( pin, GPIO_OUT );
    gpio_put( pin, 1 );

    /*-------------------------------------------------------------------------
    Power up the SPI control lines
    -------------------------------------------------------------------------*/
    gpio_init( s_io_config->spi[ BSP::SPI_LTC7871 ].sck );
    gpio_set_function( s_io_config->spi[ BSP::SPI_LTC7871 ].sck, GPIO_FUNC_SPI );
    gpio_pull_down( s_io_config->spi[ BSP::SPI_LTC7871 ].sck );

    gpio_init( s_io_config->spi[ BSP::SPI_LTC7871 ].mosi );
    gpio_set_function( s_io_config->spi[ BSP::SPI_LTC7871 ].mosi, GPIO_FUNC_SPI );
    gpio_pull_down( s_io_config->spi[ BSP::SPI_LTC7871 ].mosi );

    gpio_init( s_io_config->spi[ BSP::SPI_LTC7871 ].miso );
    gpio_set_function( s_io_config->spi[ BSP::SPI_LTC7871 ].miso, GPIO_FUNC_SPI );
    gpio_pull_up( s_io_config->spi[ BSP::SPI_LTC7871 ].miso );    // TODO BMB: This is a hack in V1. LTC SDO requires external pullup.

    pin = BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_SPI_CS0 );
    gpio_init( pin );
    gpio_set_dir( pin, GPIO_OUT );
    gpio_pull_up( pin );
    gpio_put( pin, 1 );

    /*-------------------------------------------------------------------------
    Initialize the SPI peripheral
    -------------------------------------------------------------------------*/
    auto pSPI = reinterpret_cast<spi_inst_t*>( BSP::getHardware( mb::hw::PERIPH_SPI, BSP::SPI_LTC7871 ) );
    const uint act_baud = spi_init( pSPI, SPI_BAUD );
    if( act_baud > SPI_BAUD )
    {
      Panic::throwError( Panic::ErrorCode::ERR_SYSTEM_INIT_FAIL );
    }

    spi_set_format( pSPI, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST );

    /*-------------------------------------------------------------------------
    Map error handlers for the LTC7871 driver
    -------------------------------------------------------------------------*/
    for( size_t ecode = Panic::_ERR_LTC_START; ecode < Panic::_ERR_LTC_END; ecode++ )
    {
      Panic::registerHandler( static_cast<Panic::ErrorCode>( ecode ), Private::on_ltc_error );
    }
  }


  void postSequence()
  {
    /*---------------------------------------------------------------------------
    Force proper control flow to the POST_SEQUENCE state. Don't cause a side effect
    since we don't know why this is being called.
    ---------------------------------------------------------------------------*/
    if( s_driver_mode != DriverMode::DISABLED )
    {
      return;
    }

    /*-------------------------------------------------------------------------
    Enable the LTC chip for communication. It's highly likely that these IO are
    already in the correct state, but we should be explicit about it.
    -------------------------------------------------------------------------*/
    // TODO BMB: Add these back in when HW v2 is ready.
    // gpio_put( s_io_config->gpio.ltcRun, 0 );
    // gpio_put( s_io_config->gpio.ltcPwmEn, 0 );

    /*-------------------------------------------------------------------------
    Ensure the LTC7871 is in a known state. It's not guaranteed the RP2040 and
    LTC7871 powered up together and we have no clue what happened previously.
    -------------------------------------------------------------------------*/
    s_driver_mode = DriverMode::POST_SEQUENCE;
    Private::clear_communication_fault();

    /*-------------------------------------------------------------------------
    Read in the HW strapping configuration to determine what operational
    paramters the LTC7871 decided on during power up. This ensures our software
    algorithms don't conflict with the hardware. For a trivial example, it
    would be really unfortunate if the MFR_CONFIG2_BUCK_BOOST bit was set to
    boost mode, because the software assumes we're always in buck mode.
    -------------------------------------------------------------------------*/
    const uint8_t cfg1_reg = Private::read_register( REG_MFR_CONFIG1 );
    const uint8_t cfg2_reg = Private::read_register( REG_MFR_CONFIG2 );

    // TODO BMB: May want to add configurable expected defaults depending on HW versions.
    // Some of the default current sense limits might not be the same.

    const bool is_buck_mode = ( cfg2_reg & MFR_CONFIG2_BUCK_BOOST_Msk ) == MFR_CONFIG2_BUCK_BOOST_Buck;
    Panic::assertion( is_buck_mode, Panic::ERR_LTC_HW_STRAP_FAIL );

    const bool is_10v_drvcc = ( cfg1_reg & MFR_CONFIG1_DRVCC_SET_Msk ) == MFR_CONFIG1_DRVCC_SET_10V;
    Panic::assertion( is_10v_drvcc, Panic::ERR_LTC_HW_STRAP_FAIL );

    const bool is_40mv_ilim = ( cfg1_reg & MFR_CONFIG1_ILIM_SET_Msk ) == MFR_CONFIG1_ILIM_SET_40mV;
    Panic::assertion( is_40mv_ilim, Panic::ERR_LTC_HW_STRAP_FAIL );

    const bool is_burst_mode = ( cfg2_reg & MFR_CONFIG2_BURST_Msk ) == MFR_CONFIG2_BURST_Msk;
    Panic::assertion( is_burst_mode, Panic::ERR_LTC_HW_STRAP_FAIL );

    const bool is_not_dcm_mode = ( cfg2_reg & MFR_CONFIG2_DCM_Msk ) == 0;
    Panic::assertion( is_not_dcm_mode, Panic::ERR_LTC_HW_STRAP_FAIL );

    const bool is_not_hiz_mode = ( cfg2_reg & MFR_CONFIG2_HIZ_Msk ) == 0;
    Panic::assertion( is_not_hiz_mode, Panic::ERR_LTC_HW_STRAP_FAIL );

    const bool is_not_spread_spectrum_mode = ( cfg2_reg & MFR_CONFIG2_SPRD_Msk ) == 0;
    Panic::assertion( is_not_spread_spectrum_mode, Panic::ERR_LTC_HW_STRAP_FAIL );

    /*-------------------------------------------------------------------------
    Making it here means we can transition to the next state
    -------------------------------------------------------------------------*/
    s_driver_mode = DriverMode::INITIALIZING;
  }


  void powerOn()
  {
    /*-------------------------------------------------------------------------
    Ensure we're in the correct state to power up the system
    -------------------------------------------------------------------------*/
    if( s_driver_mode != DriverMode::INITIALIZING )
    {
      // TODO: Log this as a warning
      return;
    }

    // TODO: Need to remove R102 on the board. The buffers are pulling the PWMEN line low, preventing power conversion.
    // TODO: While you're at it, may as well get R104 as well.
    /*-------------------------------------------------------------------------
    Decide if we're going to power up the system
    -------------------------------------------------------------------------*/
    Private::LTCConfig ltc;
    if( !Private::resolve_power_on_config( ltc ) )
    {
      // TODO: Log this as a warning
      s_driver_mode = DriverMode::FAULTED;
      return;
    }
  }


  void powerOff()
  {
    // Allow this to be called at any time. It should always be safe to power off.
    s_driver_mode = DriverMode::INITIALIZING;
  }


  DriverMode getMode()
  {
    return s_driver_mode;
  }


  FaultCode getFaultCode()
  {
    return s_fault_code;
  }


  void clearFaultCode()
  {
    s_fault_code = FaultCode::NO_FAULT;
  }


  bool Private::resolve_power_on_config( LTCConfig &cfg )
  {
    /*-------------------------------------------------------------------------
    *IMPORTANT* Really need to read any configuration data from the
    previous power cycle to ensure we don't boot up and immediately change
    output voltage/current limts. If a battery is connected and we reconfigure
    all willy nilly, it could be catastrophic.

    There should be a layered approach:
      1. If any of the following are true, return an invalid configuration:
        - Output voltage is present and no input voltage
        - Output voltage is higher than input voltage
        - No voltage on either the input or output
        - Any voltage is outside the expected operational range

      2. With a BMS attached, read and validate the configuration. If it's
         invalid, return false and do not boot. The BMS acts as a single
         point of truth to describe the connected storage system. It's also
         responsible for isolation from this buck converter.

      2. If the BMS is unavailable and no voltage is present on the output,
         we can assume there is no battery and power up to some sane values
         based on the input voltage. This will essentially be "free running"
         mode where we try and provide max power the solar panel can provide.

        2a. When free running, use the last programmed system limits for voltage
            and current and ramp to those points as a kind of "soft-start".
            This thing *is* still programmable via other interfaces after all.

    Log all boot decisions to the system log for later analysis.
    -------------------------------------------------------------------------*/

    return false;
  }

  void Private::clear_communication_fault()
  {
    /*-------------------------------------------------------------------------
    Clear the CML bit in the MFR_CHIP_CTRL register
    -------------------------------------------------------------------------*/
    uint8_t ctrl_reg = read_register( REG_MFR_CHIP_CTRL );
    if( ( ctrl_reg & MFR_CHIP_CTRL_CML_Msk ) == MFR_CHIP_CTRL_CML_Normal )
    {
      return;
    }

    ctrl_reg |= MFR_CHIP_CTRL_CML_Fault;
    write_register( REG_MFR_CHIP_CTRL, ctrl_reg );

    /*-------------------------------------------------------------------------
    Ensure the fault was cleared
    -------------------------------------------------------------------------*/
    ctrl_reg = read_register( REG_MFR_CHIP_CTRL );
    if( ( ctrl_reg & MFR_CHIP_CTRL_CML_Msk ) != MFR_CHIP_CTRL_CML_Normal )
    {
      Panic::throwError( Panic::ErrorCode::ERR_LTC_CMD_FAIL );
    }
  }


  void Private::reset_configuration()
  {
    /*-------------------------------------------------------------------------
    Request a reset through the MFR_CHIP_CTRL register. The datasheet claims
    this bit is sticky, so we need to set it to 1, then back to 0 to clear it.
    -------------------------------------------------------------------------*/
    write_register( REG_MFR_CHIP_CTRL, MFR_CHIP_CTRL_RESET );
    write_register( REG_MFR_CHIP_CTRL, 0u );

    /*-------------------------------------------------------------------------
    Validate all the R/W registers are in their default state (0x00)
    -------------------------------------------------------------------------*/
    const etl::array<uint8_t, 5> regs_to_check{ REG_MFR_CHIP_CTRL, REG_MFR_IDAC_VLOW, REG_MFR_IDAC_VHIGH, REG_MFR_IDAC_SETCUR,
                                                REG_MFR_SSFM };

    for( const auto &reg : regs_to_check )
    {
      const uint8_t actual = read_register( reg );
      Panic::assertion( actual == 0x00, Panic::ErrorCode::ERR_LTC_CMD_FAIL );
    }
  }


  void Private::write_register( const uint8_t reg, const uint8_t data )
  {
    const auto cs_pin = BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_SPI_CS0 );
    auto       pSPI   = reinterpret_cast<spi_inst_t*>( BSP::getHardware( mb::hw::PERIPH_SPI, BSP::SPI_LTC7871 ) );

    /*-------------------------------------------------------------------------
    Compute the buffer to send over SPI
    -------------------------------------------------------------------------*/
    spi_txfr_buffer_t tx_buf = { static_cast<uint8_t>( ( reg << 1u ) & 0xFF ), data, 0 };
    tx_buf[ LTC_PEC_IDX ]    = compute_pec( tx_buf[ LTC_ADDR_IDX ], data );

    /*-------------------------------------------------------------------------
    Perform the SPI transfer
    -------------------------------------------------------------------------*/
    gpio_put( cs_pin, 0 );
    const int write_size = spi_write_blocking( pSPI, tx_buf.data(), tx_buf.size() );
    gpio_put( cs_pin, 1 );

    if( write_size != static_cast<int>( tx_buf.size() ) )
    {
      Panic::throwError( Panic::ErrorCode::ERR_LTC_DATA_WRITE_FAIL );
    }

    /*-------------------------------------------------------------------------
    Ensure there was no errors applying the transaction
    -------------------------------------------------------------------------*/
    const auto ctrl_reg = read_register( REG_MFR_CHIP_CTRL );
    if( ( ctrl_reg & MFR_CHIP_CTRL_CML_Msk ) == MFR_CHIP_CTRL_CML_Fault )
    {
      Panic::throwError( Panic::ErrorCode::ERR_LTC_PEC_WRITE_FAIL );
    }
  }


  uint8_t Private::read_register( const uint8_t reg )
  {
    const auto cs_pin = BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_SPI_CS0 );
    auto       pSPI   = reinterpret_cast<spi_inst_t*>( BSP::getHardware( mb::hw::PERIPH_SPI, BSP::SPI_LTC7871 ) );

    /*-------------------------------------------------------------------------
    Prepare data for the read
    -------------------------------------------------------------------------*/
    spi_txfr_buffer_t rx_buf   = { 0, 0, 0 };
    const uint8_t     read_cmd = static_cast<uint8_t>( ( reg << 1u ) | 1u );

    /*-------------------------------------------------------------------------
    Perform the SPI transfer
    -------------------------------------------------------------------------*/
    gpio_put( cs_pin, 0 );
    const int read_size = spi_read_blocking( pSPI, read_cmd, rx_buf.data(), rx_buf.size() );
    gpio_put( cs_pin, 1 );

    if( read_size != static_cast<int>( rx_buf.size() ) )
    {
      Panic::throwError( Panic::ErrorCode::ERR_LTC_DATA_READ_FAIL );
    }

    /*-------------------------------------------------------------------------
    Validate the PEC code, calculated over the address we sent + the returned
    data byte.
    -------------------------------------------------------------------------*/
    const uint8_t pec = compute_pec( read_cmd, rx_buf[ LTC_DATA_IDX ] );

    if( pec != rx_buf[ LTC_PEC_IDX ] )
    {
      Panic::throwError( Panic::ErrorCode::ERR_LTC_PEC_READ_FAIL );
    }

    return rx_buf[ LTC_DATA_IDX ];
  }


  uint8_t Private::compute_pec( const uint8_t addr, const uint8_t data )
  {
    /*-------------------------------------------------------------------------
    Initialize the algorithm according to pg. 33 of the LTC7871 datasheet
    -------------------------------------------------------------------------*/
    uint16_t tmp = static_cast<uint16_t>( addr << 8u | data );
    uint16_t pec = 0x41;

    /*-------------------------------------------------------------------------
    Compute the PEC code for the given data. Must iterate MSB first since that
    is what the LTC7871 is doing (see Figure 14).
    -------------------------------------------------------------------------*/
    for( int i = 15; i >= 0; i-- )
    {
      const uint16_t din = ( tmp >> i ) & 1u;               // Get the current data bit
      const uint16_t in0 = din ^ ( ( pec >> 7u ) & 1u );    // XOR the highest PEC bit with the current data bit
      const uint16_t in1 = in0 ^ ( ( pec >> 0u ) & 1u );    // XOR lowest PEC bit with the result of the previous XOR
      const uint16_t in2 = in0 ^ ( ( pec >> 1u ) & 1u );    // XOR the second lowest PEC bit with the result of the first XOR

      // Shift PEC left by 1 and update lowest bits
      pec = ( pec << 1u ) & 0xF8u;
      pec |= ( ( in2 << 2u ) | ( in1 << 1u ) | in0 ) & 0x07u;
      pec &= 0xFFu;
    }

    return pec;
  }


  bool Private::on_ltc_error( const Panic::ErrorCode &err )
  {
    // Unrecoverable:
    // - ERR_LTC_PEC_READ_FAIL
    // - ERR_LTC_PEC_WRITE_FAIL
    // - ERR_LTC_DATA_READ_FAIL
    // - ERR_LTC_DATA_WRITE_FAIL

    // Potentially Recoverable:
    // - ERR_LTC_CMD_FAIL

    // Key off DriverMode to determine the final error behavior.
    return true;
  }
}    // namespace HW::LTC7871
