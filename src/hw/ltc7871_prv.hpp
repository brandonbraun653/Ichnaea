/******************************************************************************
 *  File Name:
 *    ltc7871_prv.hpp
 *
 *  Description:
 *    Internal private interface for the LTC7871 driver. These methods are not
 *    intended to be used outside of the driver itself, but are exposed for
 *    testing purposes.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_LTC7871_PRIVATE_HPP
#define ICHNAEA_LTC7871_PRIVATE_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>
#include "src/system/system_error.hpp"

namespace HW::LTC7871::Private
{
  /*---------------------------------------------------------------------------
  Enumerations
  ---------------------------------------------------------------------------*/

  enum SwitchingMode : uint8_t
  {
    LTC_MODE_BURST = 0x00,
    LTC_MODE_DISC  = 0x01,
    LTC_MODE_CONT  = 0x02
  };

  /*---------------------------------------------------------------------------
  Structures
  ---------------------------------------------------------------------------*/

  /**
   * @brief Register values to control power conversion setpoints
   */
  struct LTCConfig
  {
    uint8_t idac_vlow;   /* Register setting for MFR_IDAC_VLOW */
    uint8_t idac_vhigh;  /* Register setting for MFR_IDAC_VHIGH */
    uint8_t idac_setcur; /* Register setting for MFR_IDAC_SETCUR */
    uint8_t ssfm;        /* Register setting for MFR_SSFM */
  };

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Decides a safe configuration to apply to the LTC7871.
   *
   * This will resolve through several data sources to try and determine the
   * best configuration to apply to the LTC7871 when turning on the high power
   * conversion hardware. If no configuration can be determined safely, this
   * function will return false.
   *
   * @param cfg Configuration to update
   * @return true  The resolved configuration is safe
   * @return false Unable to safely resolve a configuration
   */
  bool resolve_power_on_config( LTCConfig &cfg );

  /**
   * @brief Clears the CML bit in the MFR_CHIP_CTRL register
   */
  void clear_communication_fault();

  /**
   * @brief Resets all R/W registers to their default values.
   *
   * This effectively clears all configuration settings and returns the
   * LTC7871 to a known state.
   */
  void reset_configuration();

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
  void write_register( const uint8_t reg, const uint8_t data );

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
  uint8_t read_register( const uint8_t reg );

  /**
   * @brief Computes the PEC code for the given buffer
   *
   * @param addr Address byte sent to the LTC7871, including R/W bit
   * @param data Data byte sent to the LTC7871
   * @return uint8_t PEC code
   */
  uint8_t compute_pec( const uint8_t addr, const uint8_t data );

  /**
   * @brief Error handler for PEC calculation failures
   *
   * @param err Error code that was thrown
   * @return bool True if the error was handled, false otherwise
   */
  bool on_ltc_error( const Panic::ErrorCode &err );

  /**
   * @brief Enable or disable write protection on the LTC7871 IDAC registers
   *
   * @param enable True to enable write protection, false to disable
   */
  void idac_write_protect( const bool enable );

  /**
   * @brief Sets the LTC MODE pin to one of the SwitchingMode values
   *
   * @param mode Which mode to set the pin to
   */
  void set_mode_pin( const uint8_t mode );

  /**
   * @brief Set the output voltage of the LTC7871.
   *
   * This modifies the MFR_IDAC_VLOW register to set the output voltage of the
   * LTC7871.
   *
   * @param voltage Desired setpoint in Volts
   */
  void set_output_voltage( const float voltage );

  /**
   * @brief Sets the max average current for the LTC7871.
   *
   * This modifies the MFR_IDAC_SETCUR register to set the maximum average
   * current that the LTC7871 will allow to flow through the load. Each
   * phase on the board will share the load equally.
   *
   * @param current Desired setpoint in Amps
   */
  void set_max_avg_current( const float current );

  /**
   * @brief Sets the switching frequency of the LTC7871.
   *
   * This injects a square wave signal into the SYNC pin of the LTC7871 to
   * set the switching frequency of the power conversion hardware.
   *
   * @param frequency Desired frequency in kHz
   */
  void set_switching_frequency( const float frequency );

}    // namespace HW::LTC7871::Private

#endif /* !ICHNAEA_LTC7871_PRIVATE_HPP */
