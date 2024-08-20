/******************************************************************************
 *  File Name:
 *    ltc7871_prv.cpp
 *
 *  Description:
 *    Internal driver details for the LTC7871
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "etl/array.h"
#include <etl/algorithm.h>
#include "src/bsp/board_map.hpp"
#include "src/hw/ltc7871_prv.hpp"
#include "src/hw/ltc7871_reg.hpp"
#include <mbedutils/osal.hpp>
#include <mbedutils/thread.hpp>

namespace HW::LTC7871::Private
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  static constexpr size_t LTC_ADDR_IDX = 0; /* Buffer index for register addresses */
  static constexpr size_t LTC_DATA_IDX = 1; /* Buffer index for data field */
  static constexpr size_t LTC_PEC_IDX  = 2; /* Buffer index for error check code */

  /*---------------------------------------------------------------------------
  Aliases
  ---------------------------------------------------------------------------*/

  using spi_txfr_buffer_t = etl::array<uint8_t, 3>;

  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static mb::osal::mb_recursive_mutex_t s_bus_lock; /* SPI bus lock */

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    mbed_assert( mb::osal::buildRecursiveMutexStrategy( s_bus_lock ) );
  }


  bool resolve_power_on_config( LTCConfig &cfg )
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

    Update:
    - May want to always reset the LTC7871 to OFF unless the BMS commands it to
      turn on. This might be annoying if the system suddenly resets, but it
      ensures safety.

    - May want a button input to allow someone to manually turn the system on
      without the BMS. Would be interesting. We'd have to match output voltage
      already present or just simply refuse to boot if we see output voltage.
    -------------------------------------------------------------------------*/

    return false;
  }


  void clear_communication_fault()
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


  void reset_configuration()
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


  void write_register( const uint8_t reg, const uint8_t data )
  {
    mb::thread::RecursiveLockGuard lock( s_bus_lock );

    const auto cs_pin = BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_SPI_CS0 );
    auto       pSPI   = reinterpret_cast<spi_inst_t *>( BSP::getHardware( mb::hw::PERIPH_SPI, BSP::SPI_LTC7871 ) );

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


  uint8_t read_register( const uint8_t reg )
  {
    mb::thread::RecursiveLockGuard lock( s_bus_lock );

    const auto cs_pin = BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_SPI_CS0 );
    auto       pSPI   = reinterpret_cast<spi_inst_t *>( BSP::getHardware( mb::hw::PERIPH_SPI, BSP::SPI_LTC7871 ) );

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


  uint8_t compute_pec( const uint8_t addr, const uint8_t data )
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


  bool on_ltc_error( const Panic::ErrorCode &err )
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


  void idac_write_protect( const bool enable )
  {
    const uint8_t reg = enable ? MFR_CHIP_CTRL_WP_Enable : MFR_CHIP_CTRL_WP_Disable;
    write_register( REG_MFR_CHIP_CTRL, reg );
  }


  void set_mode_pin( const uint8_t mode )
  {
    uint ccm_pin = BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_LTC_CCM );
    uint dcm_pin = BSP::getPin( mb::hw::PERIPH_GPIO, BSP::GPIO_LTC_DCM );

    switch( mode )
    {
      case SwitchingMode::LTC_MODE_CONT:
        gpio_put( ccm_pin, 1 );
        gpio_put( dcm_pin, 0 );
        break;

      case SwitchingMode::LTC_MODE_DISC:
        gpio_put( ccm_pin, 0 );
        gpio_put( dcm_pin, 1 );
        break;

      case SwitchingMode::LTC_MODE_BURST:
      default:
        gpio_put( ccm_pin, 0 );
        gpio_put( dcm_pin, 0 );
        break;
    }
  }


  void set_output_voltage( const LTCState &state, const float voltage )
  {
    /*-------------------------------------------------------------------------
    Ensure the input voltage is within the valid range
    -------------------------------------------------------------------------*/
    if( !mbed_assert_continue_msg( voltage > state.msr_input_voltage, "Cannot set voltage to %.2f when input is %.2f", voltage,
                                   state.msr_input_voltage ) )
    {
      return;
    }

    /*-------------------------------------------------------------------------
    Compute the new IDAC VLOW register value
    -------------------------------------------------------------------------*/
    uint8_t idac_vlow = compute_idac_vlow( voltage, state.vlow_ra, state.vlow_rb );
    if( idac_vlow == LTC_IDAC_REG_INVALID )
    {
      mbed_assert_continue_msg( false, "Invalid IDAC VLOW. Ra: %.2f, Rb: %.2f, Vlow: %.2f",
                                state.vlow_ra, state.vlow_rb, voltage );
      return;
    }

    write_register( REG_MFR_IDAC_VLOW, idac_vlow );
  }


  void set_switching_frequency( const float frequency )
  {
    auto pin          = BSP::getPin( mb::hw::PERIPH_PWM, BSP::PWM_LTC_SYNC );
    auto sync_channel = pwm_gpio_to_channel( pin );
    auto sync_slice   = pwm_gpio_to_slice_num( pin );

    uint16_t approx_wrap    = static_cast<uint16_t>( LTC_SYNC_PWM_FREQ / frequency ) - 1u;
    uint16_t clamped_wrap   = etl::clamp( approx_wrap, LTC_SYNC_CNT_WRAP_HF_MIN, LTC_SYNC_CNT_WRAP_LF_MAX );
    uint16_t midpoint_level = clamped_wrap / 2u;

    pwm_set_chan_level( sync_slice, sync_channel, midpoint_level );
    pwm_set_wrap( sync_slice, clamped_wrap );
  }


  uint8_t compute_idac_vlow( const float vlow, const float ra, const float rb )
  {
    static constexpr int32_t IDAC_MIN_UA = -64;
    static constexpr int32_t IDAC_MAX_UA = 63;

    /*-------------------------------------------------------------------------
    Ensure the input parameters are valid. If not, return an impossible value.
    -------------------------------------------------------------------------*/
    if( vlow < 0.0f || ra <= 0.0f || rb <= 0.0f )
    {
      return LTC_IDAC_REG_INVALID;
    }

    /*-------------------------------------------------------------------------
    Compute a realizable IDAC adjustment current (pg. 17)
    -------------------------------------------------------------------------*/
    int32_t idac_ideal_uA  = static_cast<int32_t>( 1e6 * ( ( 1.2f * ( ( ra + rb ) / ra ) - vlow ) / rb ) );
    int32_t idac_actual_uA = etl::clamp( idac_ideal_uA, IDAC_MIN_UA, IDAC_MAX_UA );

    /*-------------------------------------------------------------------------
    Convert the current reference to the 7-bit register value (pg. 39)
    -------------------------------------------------------------------------*/
    if( idac_actual_uA >= 0 )
    {
      return static_cast<uint8_t>( idac_actual_uA ) & 0x7F;
    }
    else
    {
      // 2's complement conversion
      uint8_t abs_value = static_cast<uint8_t>( -idac_actual_uA - 1u );
      return ( ~abs_value ) & 0x7F;
    }
  }


  void update_operating_point( const LTCState &state )
  {
    set_switching_frequency( 80e3 );
    set_mode_pin( SwitchingMode::LTC_MODE_DISC );

    // Control:
    //  - switching frequency
    //  - conduction mode (burst, dcm, ccm)

    // Most of this seems to be dependent on valid average current measurements.
    // Probably should do some kind of calibration on boot?
  }
}    // namespace HW::LTC7871::Private
