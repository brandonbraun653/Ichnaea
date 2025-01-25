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
#include <algorithm>
#include <mbedutils/interfaces/time_intf.hpp>
#include <mbedutils/logging.hpp>
#include <mbedutils/osal.hpp>
#include <mbedutils/threading.hpp>
#include <mbedutils/util.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/pdi/ltc7871.hpp>
#include <src/app/pdi/pgood_monitor_timeout_ms.hpp>
#include <src/app/pdi/system_current_output_rated_limit.hpp>
#include <src/app/pdi/system_voltage_output_rated_limit.hpp>
#include <src/app/pdi/target_system_current_output.hpp>
#include <src/app/pdi/target_system_voltage_output.hpp>
#include <src/bsp/board_map.hpp>
#include <src/hw/led.hpp>
#include <src/hw/ltc7871.hpp>
#include <src/hw/ltc7871_prv.hpp>
#include <src/hw/ltc7871_reg.hpp>
#include <src/system/system_db.hpp>
#include <src/system/system_error.hpp>
#include <src/system/system_sensor.hpp>

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
  static Private::LTCState    s_ltc_state;

  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/
  static uint32_t    read_faults();
  static const char *ltcFaultCodeToString( const uint32_t code );
  static void        set_output_voltage( const float voltage );
  static void        set_max_avg_current( const float current );
  static void        update_operating_point();

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void driver_init()
  {
    /*-------------------------------------------------------------------------
    Initialize the driver state
    -------------------------------------------------------------------------*/
    s_io_config = &BSP::getIOConfig();

    clear_struct( s_ltc_state );
    s_ltc_state.vlow_ra           = s_io_config->ltc_vlow_ra;
    s_ltc_state.vlow_rb           = s_io_config->ltc_vlow_rb;
    s_ltc_state.driver_mode       = DriverMode::DISABLED;
    s_ltc_state.fault_bits        = 0;
    s_ltc_state.fault_code_logged = 0;

    Private::initialize();

    /*-------------------------------------------------------------------------
    Power up the GPIO control lines, de-energizing the power stage for safety.
    -------------------------------------------------------------------------*/
    uint pin = 0;

    if( BSP::getBoardRevision() >= 2 )
    {
      /*-----------------------------------------------------------------------
      The LTC7871 has a PWMEN pin that can be used to enable/disable the power
      stage drivers. Ensure it's disabled to prevent energizing the output
      without knowing the state of the system.
      -----------------------------------------------------------------------*/
      pin = BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_LTC_PWMEN );
      gpio_init( pin );
      gpio_set_dir( pin, GPIO_OUT );
      gpio_put( pin, 1 );    // Drives a mosfet pulldown on the PWMEN pin when active

      /*-----------------------------------------------------------------------
      The LTC7871 has a RUN pin that can be used to enable/disable the entire
      controller. Ensure it's enabled to allow the LTC to power the RP2040. We
      bootstrap from a regulator on the LTC itself, supplied via the solar
      input.
      -----------------------------------------------------------------------*/
      pin = BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_LTC_RUN );
      gpio_init( pin );
      gpio_set_dir( pin, GPIO_OUT );
      gpio_put( pin, 0 );    // Drives a mosfet pulldown on the RUN pin when active
    }

    /*-------------------------------------------------------------------------
    Initialize the mode control pins and set to BURST.
    -------------------------------------------------------------------------*/
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
    const float f_clk_peri   = static_cast<float>( frequency_count_khz( CLOCKS_FC0_SRC_VALUE_CLK_PERI ) ) * 1000.0f;
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

    if( BSP::getBoardRevision() == 1 )
    {
      // V1 has a PCB issue where we need the software to pull up the MISO line.
      gpio_pull_up( s_io_config->spi[ BSP::SPI_LTC7871 ].miso );
    }

    pin = BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_SPI_CS_LTC );
    gpio_init( pin );
    gpio_set_dir( pin, GPIO_OUT );
    gpio_pull_up( pin );
    gpio_put( pin, 1 );

    /*-------------------------------------------------------------------------
    Initialize the SPI peripheral
    -------------------------------------------------------------------------*/
    auto       pSPI     = reinterpret_cast<spi_inst_t *>( BSP::getHardware( mb::hw::PERIPH_SPI, BSP::SPI_LTC7871 ) );
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
      auto cb = Panic::ErrorCallback::create<Private::on_ltc_error>();
      Panic::registerHandler( static_cast<Panic::ErrorCode>( ecode ), cb );
    }
  }


  void driver_deinit()
  {
    /*-------------------------------------------------------------------------
    Deinitialize the driver state
    -------------------------------------------------------------------------*/
    clear_struct( s_ltc_state );

    /*-------------------------------------------------------------------------
    Disable the LTC chip
    -------------------------------------------------------------------------*/
    Private::set_pwmen_pin( false );
    Private::set_run_pin( false );

    /*-------------------------------------------------------------------------
    Power down the SPI control lines
    -------------------------------------------------------------------------*/
    gpio_put( s_io_config->spi[ BSP::SPI_LTC7871 ].sck, 0 );
    gpio_put( s_io_config->spi[ BSP::SPI_LTC7871 ].mosi, 0 );
    gpio_put( s_io_config->spi[ BSP::SPI_LTC7871 ].miso, 0 );
    gpio_put( BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_SPI_CS_LTC ), 0 );

    /*-------------------------------------------------------------------------
    Power down the LTC SYNC pin
    -------------------------------------------------------------------------*/
    const uint pin   = BSP::getPin( mb::hw::PERIPH_PWM, BSP::PWM_LTC_SYNC );
    const uint slice = pwm_gpio_to_slice_num( pin );
    pwm_set_enabled( slice, false );

    /*-------------------------------------------------------------------------
    Power down the GPIO control lines
    -------------------------------------------------------------------------*/
    if( BSP::getBoardRevision() >= 2 )
    {
      gpio_put( BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_LTC_PWMEN ), 0 );
      gpio_put( BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_LTC_RUN ), 0 );
    }

    gpio_put( BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_LTC_CCM ), 0 );
    gpio_put( BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_LTC_DCM ), 0 );

    /*-------------------------------------------------------------------------
    Deinitialize the SPI peripheral
    -------------------------------------------------------------------------*/
    auto pSPI = reinterpret_cast<spi_inst_t *>( BSP::getHardware( mb::hw::PERIPH_SPI, BSP::SPI_LTC7871 ) );
    spi_deinit( pSPI );
  }


  void postSequence()
  {
    /*-------------------------------------------------------------------------
    Register the PDI keys for the LTC7871 driver
    -------------------------------------------------------------------------*/
    App::PDI::pdi_register_key__config_ltc_phase_inductor_dcr();
  }


  DriverMode getMode()
  {
    return s_ltc_state.driver_mode;
  }


  uint32_t getFaults()
  {
    return s_ltc_state.fault_bits;
  }


  void clearFaults()
  {
    /*-------------------------------------------------------------------------
    Doesn't make sense to run this function in a non-faulted state
    -------------------------------------------------------------------------*/
    if( s_ltc_state.driver_mode != DriverMode::FAULTED )
    {
      return;
    }

    /*-------------------------------------------------------------------------
    Best we can do is clear the fault bits in software. The LTC7871 will only
    report HW fault conditions that aren't clearable by software.
    -------------------------------------------------------------------------*/
    s_ltc_state.fault_bits        = 0;
    s_ltc_state.fault_code_logged = 0;

    /*-------------------------------------------------------------------------
    The only clearable "warning" from the datasheet is a communication PEC
    error, which isn't likely to be happening. Clear anyway for good measure.
    -------------------------------------------------------------------------*/
    Private::clear_communication_fault();

    /*-------------------------------------------------------------------------
    Reset indicators of a fault state and prepare for a power on sequence. This
    will quickly go back to a faulted state if the LTC7871 is still in a fault
    condition.
    -------------------------------------------------------------------------*/
    s_ltc_state.driver_mode = DriverMode::DISABLED;
    HW::LED::disable( HW::LED::Channel::FAULT );
  }


  bool enablePowerConverter( const float vout, const float iout )
  {
    /*---------------------------------------------------------------------------
    Force proper control flow to the POST_SEQUENCE state. Don't cause a side effect
    since we don't know why this is being called.
    ---------------------------------------------------------------------------*/
    if( s_ltc_state.driver_mode != DriverMode::DISABLED )
    {
      return false;
    }

    /*-------------------------------------------------------------------------
    Ensure the LTC7871 is in a known state. It's not guaranteed the RP2040 and
    LTC7871 powered up together and we have no clue what happened previously.
    -------------------------------------------------------------------------*/
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

    const bool is_buck_mode = ( cfg2_reg & MFR_CONFIG2_BUCK_BOOST_Msk ) == MFR_CONFIG2_BUCK_BOOST_Buck;
    Panic::assertion( is_buck_mode, Panic::ERR_LTC_HW_STRAP_FAIL );

    const bool is_10v_drvcc = ( cfg1_reg & MFR_CONFIG1_DRVCC_SET_Msk ) == MFR_CONFIG1_DRVCC_SET_10V;
    Panic::assertion( is_10v_drvcc, Panic::ERR_LTC_HW_STRAP_FAIL );

    const bool is_not_dcm_mode = ( cfg2_reg & MFR_CONFIG2_DCM_Msk ) == 0;
    Panic::assertion( is_not_dcm_mode, Panic::ERR_LTC_HW_STRAP_FAIL );

    const bool is_not_hiz_mode = ( cfg2_reg & MFR_CONFIG2_HIZ_Msk ) == 0;
    Panic::assertion( is_not_hiz_mode, Panic::ERR_LTC_HW_STRAP_FAIL );

    const bool is_not_spread_spectrum_mode = ( cfg2_reg & MFR_CONFIG2_SPRD_Msk ) == 0;
    Panic::assertion( is_not_spread_spectrum_mode, Panic::ERR_LTC_HW_STRAP_FAIL );

    if( BSP::getBoardRevision() >= 2 )
    {
      const bool is_40mv_ilim = ( cfg1_reg & MFR_CONFIG1_ILIM_SET_Msk ) == MFR_CONFIG1_ILIM_SET_40mV;
      Panic::assertion( is_40mv_ilim, Panic::ERR_LTC_HW_STRAP_FAIL );
    }

    /*-------------------------------------------------------------------------
    Check for any pending fault states before proceeding
    -------------------------------------------------------------------------*/
    const uint32_t faults = read_faults();
    for( size_t i = 0; i < LTCFaultBits::LTC_FAULT_COUNT; i++ )
    {
      LOG_ERROR_IF( ( ( faults & ( 1u << i ) ) != 0 ), "LTC7871 Fault: %s", ltcFaultCodeToString( i ) );
    }

    if( faults != 0 )
    {
      s_ltc_state.driver_mode = DriverMode::FAULTED;
      mbed_assert_continue_msg( false, "LTC7871 fault code present before power on: %d", faults );
      return false;
    }

    /*-------------------------------------------------------------------------
    Update the system state
    -------------------------------------------------------------------------*/
    runStateUpdater();

    switch( cfg1_reg & MFR_CONFIG1_ILIM_SET_Msk )
    {
      case MFR_CONFIG1_ILIM_SET_10mV:    // 0V
      case MFR_CONFIG1_ILIM_SET_20mV:    // 1/4 V5
        s_ltc_state.ilim_gain_k = 40.0f;
        break;

      case MFR_CONFIG1_ILIM_SET_30mV:    // Float
      case MFR_CONFIG1_ILIM_SET_40mV:    // 3/4 V5
      case MFR_CONFIG1_ILIM_SET_50mV:    // V5
        s_ltc_state.ilim_gain_k = 20.0f;
        break;
    }

    /*-------------------------------------------------------------------------
    Validate minimum on-time requirements (pg. 29)
    -------------------------------------------------------------------------*/
    if( !Private::min_on_time_satisfied( vout, s_ltc_state.msr_input_voltage ) )
    {
      s_ltc_state.driver_mode = DriverMode::DISABLED;
      mbed_assert_continue_msg( false, "Vin/Vout ratio too high. Cannot power on." );
      return false;
    }

    /*-------------------------------------------------------------------------
    Program the LTC7871 registers using the configuration options
    -------------------------------------------------------------------------*/
    /* Start off in discontinuous mode, which can handle light/no loads */
    Private::set_mode_pin( Private::SwitchingMode::LTC_MODE_DISC );

    /* Program high level setpoints */
    set_max_avg_current( iout );
    set_output_voltage( vout );

    /*-------------------------------------------------------------------------
    Enable the LTC7871 power stage and wait for the PGOOD signal
    -------------------------------------------------------------------------*/
    Private::set_pwmen_pin( true );

    size_t   start_time  = mb::time::millis();
    size_t   timeout     = std::max( App::PDI::getPgoodMonitorTimeoutMS(), ( uint32_t )10 );
    bool     power_good  = false;
    uint8_t  post_status = 0;
    uint32_t post_faults = 0;

    while( !power_good && ( ( mb::time::millis() - start_time ) < timeout ) )
    {
      post_status = Private::read_register( REG_MFR_STATUS );
      post_faults = read_faults();
      power_good  = ( ( post_faults == 0 ) && ( ( post_status & MFR_STATUS_PGOOD_Msk ) == MFR_STATUS_PGOOD_Msk ) );
    }

    /*-------------------------------------------------------------------------
    Revert to a safe state if the LTC7871 failed to power up
    -------------------------------------------------------------------------*/
    if( !power_good )
    {
      Private::set_pwmen_pin( false );
      s_ltc_state.driver_mode = DriverMode::FAULTED;
      mbed_assert_continue_msg( false, "LTC7871 failed power up POST sequence: 0x%02X, 0x%08X", post_status, post_faults );
      return false;
    }

    s_ltc_state.driver_mode = DriverMode::ENABLED;
    return true;
  }


  void disablePowerConverter()
  {
    /*-------------------------------------------------------------------------
    Disable the LTC7871 power stage and wait for PGOOD to go low
    -------------------------------------------------------------------------*/
    Private::set_pwmen_pin( false );

    size_t  start_time  = mb::time::millis();
    size_t  timeout     = std::max( App::PDI::getPgoodMonitorTimeoutMS(), ( uint32_t )10 );
    bool    power_good  = true;
    uint8_t post_status = 0;

    while( power_good && ( ( mb::time::millis() - start_time ) < timeout ) )
    {
      post_status = Private::read_register( REG_MFR_STATUS );
      power_good  = ( post_status & MFR_STATUS_PGOOD_Msk ) == MFR_STATUS_PGOOD_Msk;
    }

    if( power_good )
    {
      Panic::throwError( Panic::ErrorCode::ERR_LTC_PWR_DWN_FAIL );
    }
    else
    {
      s_ltc_state.driver_mode = DriverMode::DISABLED;
    }
  }


  void setVoutRef( const float voltage )
  {
    bool accepted = true;

    accepted &= Private::min_on_time_satisfied( voltage, s_ltc_state.msr_input_voltage );
    LOG_WARN_IF( !accepted, "Vin/Vout ratio too high. Cannot set voltage to %.2f", voltage );

    if( accepted )
    {
      set_output_voltage( voltage );
      update_operating_point();
    }
  }


  void setIoutRef( const float current )
  {
    set_max_avg_current( current );
    update_operating_point();
  }


  float getAverageOutputCurrent( float voltage )
  {
    constexpr float IMON_RNG_MIN = 0.4f;
    constexpr float IMON_RNG_MAX = 2.5f;
    constexpr float IMON_ZERO    = 1.25f;

    /*-------------------------------------------------------------------------
    Calculate the current based on the IMON voltage (pg. 16)
    -------------------------------------------------------------------------*/
    if( ( voltage < IMON_RNG_MIN ) || ( voltage > IMON_RNG_MAX ) )
    {
      mbed_assert_continue_msg( s_ltc_state.driver_mode != DriverMode::ENABLED, "IMON voltage out of range: %.2f", voltage );
      voltage = std::min( IMON_RNG_MAX, std::max( IMON_RNG_MIN, voltage ) );
    }

    float denom = static_cast<float>( s_ltc_state.ilim_gain_k ) * App::PDI::getLTCPhaseInductorDCR();
    float num   = 6.0f * ( voltage - IMON_ZERO );

    return num / denom;
  }


  void runFaultMonitoring()
  {
    /*-------------------------------------------------------------------------
    Check for any pending fault states
    -------------------------------------------------------------------------*/
    s_ltc_state.fault_bits = read_faults();

    for( size_t i = 0; i < LTCFaultBits::LTC_FAULT_COUNT; i++ )
    {
      if( ( ( s_ltc_state.fault_bits & ( 1u << i ) ) != 0 )                // Fault is present
          && ( ( s_ltc_state.fault_code_logged & ( 1u << i ) ) == 0 ) )    // Fault has not been logged yet
      {
        /*---------------------------------------------------------------------
        Disable the power stage immediately to prevent further damage
        ---------------------------------------------------------------------*/
        Private::set_pwmen_pin( false );

        /*---------------------------------------------------------------------
        Log the fault code
        ---------------------------------------------------------------------*/
        s_ltc_state.fault_code_logged |= ( 1u << i );
        LOG_ERROR( "LTC7871 Fault: %s", ltcFaultCodeToString( i ) );
      }
    }

    /*-------------------------------------------------------------------------
    If any faults are present, transition the system to a faulted state
    -------------------------------------------------------------------------*/
    if( ( s_ltc_state.fault_bits != 0 ) && ( s_ltc_state.driver_mode != DriverMode::FAULTED ) )
    {
      /*-----------------------------------------------------------------------
      Run the full power down sequence with a bit more breathing room.
      -----------------------------------------------------------------------*/
      disablePowerConverter();

      /*-----------------------------------------------------------------------
      Transition the system to a faulted state
      -----------------------------------------------------------------------*/
      LOG_ERROR( "LTC7871 transition to faulted state" );
      s_ltc_state.driver_mode = DriverMode::FAULTED;
    }
  }


  void runStateUpdater()
  {
    using namespace System::Sensor;

    s_ltc_state.msr_input_voltage     = getMeasurement( Element::VMON_SOLAR_INPUT );
    s_ltc_state.msr_output_voltage    = getMeasurement( Element::VMON_LOAD );
    s_ltc_state.msr_immediate_current = getMeasurement( Element::IMON_LOAD );
    s_ltc_state.msr_average_current   = getMeasurement( Element::IMON_LTC_AVG );
  }


  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Converts a fault code to a human readable string
   *
   * @param code  Fault code to convert
   * @return Loggable/printable error message of the code
   */
  static const char *ltcFaultCodeToString( const uint32_t code )
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


  /**
   * @brief Reads the fault registers of the LTC7871.
   *
   * @return uint32_t
   */
  static uint32_t read_faults()
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


  static void set_output_voltage( const float voltage )
  {
    /*-------------------------------------------------------------------------
    Ensure the input voltage is within the valid range
    -------------------------------------------------------------------------*/
    if( !mbed_assert_continue_msg( voltage < s_ltc_state.msr_input_voltage,
                                   "Requested output voltage (%.2f V) exceeds input voltage (%.2f V).", voltage,
                                   s_ltc_state.msr_input_voltage ) )
    {
      return;
    }

    /*-------------------------------------------------------------------------
    Compute the new IDAC VLOW register value
    -------------------------------------------------------------------------*/
    uint8_t idac_vlow = Private::compute_idac_vlow( voltage, s_ltc_state.vlow_ra, s_ltc_state.vlow_rb );
    if( idac_vlow == Private::LTC_IDAC_REG_INVALID )
    {
      mbed_assert_continue_msg( false, "Invalid IDAC VLOW. Ra: %.2f, Rb: %.2f, Vlow: %.2f", s_ltc_state.vlow_ra,
                                s_ltc_state.vlow_rb, voltage );
      return;
    }

    /*-------------------------------------------------------------------------
    Program the new output voltage
    -------------------------------------------------------------------------*/
    Private::idac_write_protect( false );
    Private::write_register( REG_MFR_IDAC_VLOW, idac_vlow );
    Private::idac_write_protect( true );
  }


  static void set_max_avg_current( const float current )
  {
    /*-------------------------------------------------------------------------
    Compute the next register value
    -------------------------------------------------------------------------*/
    uint8_t new_idac_value =
        Private::compute_idac_setcur( s_ltc_state.ilim_gain_k, current, App::PDI::getLTCPhaseInductorDCR() );
    if( new_idac_value == Private::LTC_IDAC_REG_INVALID )
    {
      mbed_assert_continue_msg( false, "Invalid current request: %.2f", current );
      return;
    }

    /*-------------------------------------------------------------------------
    Program the new current limit
    -------------------------------------------------------------------------*/
    Private::idac_write_protect( false );
    Private::write_register( REG_MFR_IDAC_SETCUR, new_idac_value );
    Private::idac_write_protect( true );
  }


  static void update_operating_point()
  {
    /*-------------------------------------------------------------------------
    Get the latest information from the monitor
    -------------------------------------------------------------------------*/
    runStateUpdater();

    Private::set_switching_frequency( 100e3 );
    Private::set_mode_pin( Private::SwitchingMode::LTC_MODE_DISC );

    // TODO BMB: This is where the magic happens. We need to implement a control
    // algorithm that can adjust the LTC7871 operating point based on the
    // current system state. This will likely involve some kind of look up table
    // with PDI programmable setpoints.

    // Control:
    //  - switching frequency
    //  - conduction mode (burst, dcm, ccm)

    // Most of this seems to be dependent on valid average current measurements.
    // Probably should do some kind of calibration on boot?

    // Scenarios:
    // - Low current, high output voltage: 54V, 0.5A
    // - High current, high output voltage: 54V, 90A
    // - Low current, low output voltage: 12V, 0.5A
    // - High current, low output voltage: 12V, 100A
    // - Medium current, medium output voltage: 24V, 10A
  }
}    // namespace HW::LTC7871
