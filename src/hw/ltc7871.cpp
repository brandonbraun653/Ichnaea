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
#include "src/bsp/board_map.hpp"
#include "src/hw/ltc7871.hpp"
#include "src/hw/ltc7871_prv.hpp"
#include "src/hw/ltc7871_reg.hpp"
#include "src/system/system_error.hpp"
#include <mbedutils/logging.hpp>

namespace HW::LTC7871
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  /**
   * @brief Base clock for the SPI interface to the LTC7871.
   * @warning Max clock for LTC7871 is 5MHz.
   */
  static constexpr size_t LTC_SPI_CLK_RATE = 1'000'000;

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

    /*-------------------------------------------------------------------------
    Configure the LTC SYNC pin. The LTC boots off the FREQ pin resistor, set
    to ~400kHz. The fact that we're running this code means the LTC is already
    in a known state (it's powering the RP2040), so match the switching
    frequency on the SYNC pin to minimize disturbances.
    -------------------------------------------------------------------------*/
    pin                      = BSP::getPin( mb::hw::PERIPH_PWM, BSP::PWM_LTC_SYNC );
    const float f_clk_peri   = static_cast<float>( frequency_count_khz( CLOCKS_FC0_SRC_VALUE_CLK_PERI ) );
    const float divisor      = f_clk_peri / Private::LTC_SYNC_PWM_FREQ;
    const uint  sync_channel = pwm_gpio_to_channel( pin );
    const uint  sync_slice   = pwm_gpio_to_slice_num( pin );

    /* Map GPIO to PWM */
    gpio_init( pin );
    gpio_set_function( pin, GPIO_FUNC_PWM );
    gpio_set_pulls( pin, false, true );

    /* Configure the PWM to idle state */
    pwm_set_wrap( sync_slice, Private::LTC_SYNC_CNT_WRAP_LF_MAX );
    pwm_set_chan_level( sync_slice, sync_channel, Private::LTC_SYNC_CNT_WRAP_OFF );
    pwm_set_clkdiv( sync_slice, divisor );
    pwm_set_counter( sync_slice, 0 );
    pwm_set_enabled( sync_slice, true );

    /* Match the base frequency of ~400kHz */
    // TODO: Should this be moved to later? It's not clear if the LTC is ready for this yet.
    Private::set_switching_frequency( 400'000.0f );

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
    const uint act_baud = spi_init( pSPI, LTC_SPI_CLK_RATE );
    if( act_baud > LTC_SPI_CLK_RATE )
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
    LOG_DEBUG( "LTC7871 Strap: CFG1=0x%02X, CFG2=0x%02X", cfg1_reg, cfg2_reg );

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
    Process any fault states
    -------------------------------------------------------------------------*/
    const uint32_t faults = readLTCFaults();
    for( size_t i = 0; i < LTCFaultBits::LTC_FAULT_COUNT; i++ )
    {
      LOG_ERROR_IF( ( ( faults & ( 1u << i ) ) != 0 ), "LTC7871 Fault: %s", ltcFaultCodeToString( i ) );
    }
    Panic::assertion( faults == 0, Panic::ERR_LTC_FAULT );

    /*-------------------------------------------------------------------------
    Summarize the status register
    -------------------------------------------------------------------------*/
    const uint8_t status = Private::read_register( REG_MFR_STATUS );
    LOG_DEBUG( "LTC7871 Status: 0x%02X, SS_DONE: %d, MAX_CURRENT: %d, PGOOD: %d", status,
               ( status & MFR_STATUS_SS_DONE_Msk ) != 0, ( status & MFR_STATUS_MAX_CURRENT_Msk ) != 0,
               ( status & MFR_STATUS_PGOOD_Msk ) != 0 );

    /*-------------------------------------------------------------------------
    // !TESTING
    Set the output voltage to 12V
    -------------------------------------------------------------------------*/
    Private::set_mode_pin( Private::LTC_MODE_CONT );

    Private::idac_write_protect( false );
    Private::write_register( REG_MFR_IDAC_VLOW, 0x3F );
    Private::idac_write_protect( true );


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


  uint32_t readLTCFaults()
  {
    /*-------------------------------------------------------------------------
    Read the fault registers from the LTC7871
    -------------------------------------------------------------------------*/
    const uint8_t fault1 = Private::read_register( REG_MFR_FAULT );
    const uint8_t fault2 = Private::read_register( REG_MFR_OC_FAULT );
    const uint8_t fault3 = Private::read_register( REG_MFR_NOC_FAULT );

    /*-------------------------------------------------------------------------
    Collapse the fault bits into a single 32-bit value
    -------------------------------------------------------------------------*/
    uint32_t bits = 0;
    bits |= static_cast<uint32_t>( fault1 ) << 16u;
    bits |= static_cast<uint32_t>( fault2 ) << 8u;
    bits |= static_cast<uint32_t>( fault3 );

    return bits;
  }


  uint32_t readLTCStatus()
  {
    /*-------------------------------------------------------------------------
    Read the status register from the LTC7871
    -------------------------------------------------------------------------*/
    const uint8_t status = Private::read_register( REG_MFR_STATUS );

    return static_cast<uint32_t>( status );
  }


  const char *ltcFaultCodeToString( const uint32_t code )
  {
    switch( code )
    {
      case LTC_FAULT_OVERTEMP:
        return "Over-temperature";

      case LTC_FAULT_VREF_BAD:
        return "Internal reference voltage out of range";

      case LTC_FAULT_V5_UV:
        return "5V output under-voltage";

      case LTC_FAULT_DRVCC_UV:
        return "DRVCC under-voltage";

      case LTC_FAULT_VHIGH_UV:
        return "VHigh under-voltage sense < 1.2v";

      case LTC_FAULT_VHIGH_OV:
        return "VHigh over-voltage sense > 1.2v";

      case LTC_FAULT_VLOW_OV:
        return "VLow over-voltage sense > 1.2v";

      case LTC_FAULT_OC_1:
        return "Channel 1 over-current";

      case LTC_FAULT_OC_2:
        return "Channel 2 over-current";

      case LTC_FAULT_OC_3:
        return "Channel 3 over-current";

      case LTC_FAULT_OC_4:
        return "Channel 4 over-current";

      case LTC_FAULT_OC_5:
        return "Channel 5 over-current";

      case LTC_FAULT_OC_6:
        return "Channel 6 over-current";

      case LTC_FAULT_NOC_1:
        return "Channel 1 negative over-current";

      case LTC_FAULT_NOC_2:
        return "Channel 2 negative over-current";

      case LTC_FAULT_NOC_3:
        return "Channel 3 negative over-current";

      case LTC_FAULT_NOC_4:
        return "Channel 4 negative over-current";

      case LTC_FAULT_NOC_5:
        return "Channel 5 negative over-current";

      case LTC_FAULT_NOC_6:
        return "Channel 6 negative over-current";

      default:
        return "Unknown fault code";
    }
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


  ILim getILim()
  {
    return ILim::V3_4;
  }


  float getRSense()
  {
    return 0.0016f;
  }

}    // namespace HW::LTC7871
