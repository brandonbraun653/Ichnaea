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
#include <src/system/system_error.hpp>
#include <src/hw/ltc7871.hpp>
#include <mbedutils/osal.hpp>

namespace HW::LTC7871::Private
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  /**
   * @brief Base clock to configure the PWM peripheral for LTC sync signal
   */
  static constexpr float LTC_SYNC_PWM_FREQ = 10'000'000.0f;

  /**
   * @brief Max counter value to ensure minimum switching frequency is met.
   *
   * Assuming a counter base clock of 10MHz and minimum switching frequency
   * of 60kHz, this is the max value the counter can wrap at before the
   * switching frequency is too low.
   *
   * Period @60kHz: 16.666 uS
   * Base Clock: 100nS
   * Counter Value: 166 == 16.6 uS or 60.240kHz
   */
  static constexpr uint16_t LTC_SYNC_CNT_WRAP_LF_MAX = 166u;

  /**
   * @brief Min counter value to ensure maximum switching frequency is met.
   *
   * Assuming a counter base clock of 10MHz and maximum switching frequency
   * of 750kHz, this is the min value the counter can wrap at before the
   * switching frequency is too high.
   *
   * Period @750kHz: 1.333 uS
   * Base Clock: 100nS
   * Counter Value: 14 == 1.4 uS or 714.285kHz
   */
  static constexpr uint16_t LTC_SYNC_CNT_WRAP_HF_MIN = 14u;

  /**
   * @brief Default value to disable signaling on the LTC SYNC pin
   */
  static constexpr uint16_t LTC_SYNC_CNT_WRAP_OFF = LTC_SYNC_CNT_WRAP_LF_MAX + 1u;

  /**
   * @brief Indicate an invalid computation for the LTC IDAC registers
   */
  static constexpr uint8_t LTC_IDAC_REG_INVALID = 0xFF;

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
   * @brief Internal state of the LTC7871 driver
   */
  struct LTCState
  {
    /* Static Board Properties */
    float vlow_ra;     /* Bottom resistor in VLow feedback divider */
    float vlow_rb;     /* Top resistor in VLow feedback divider */
    float ilim_gain_k; /**< K gain constant based on ILIM setting (pg. 16) */

    /* Measurement Information */
    float msr_input_voltage;     /* Measured input voltage (volts) */
    float msr_output_voltage;    /* Measured output voltage (volts) */
    float msr_average_current;   /* Measured average current (amps) */
    float msr_immediate_current; /* Measured output current (amps) */

    /* Status */
    DriverMode driver_mode;       /* Current operational mode of the driver */
    uint32_t   fault_bits;        /* Bitfield to indicate if a fault has occurred */
    uint32_t   fault_code_logged; /* Bitfield to indicate if a fault has been logged */
  };

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initialize the internal driver state for the LTC7871.
   */
  void initialize();

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
   * @brief Set the state of the LTC RUN pin.
   *
   * This controls the logic level present on pin 27 of the LTC7871. This will either
   * enable or disable the controller logic.
   *
   * @param enable True to set RUN logic high, false to set RUN logic low
   */
  void set_run_pin( const bool enable );

  /**
   * @brief Enable or disable the power stage drivers.
   *
   * The LTC7871 normally controls pin 28 (PWMEN) to activate the power stage, however
   * this can be overridden by the user with an attached pulldown. This funtion acts
   * like a half-gate mechanism. Disabling forces PWMEN low, but enabling only allows the
   * LTC7871 to control the pin if it wants to. It does not force any other state.
   *
   * @param enable False to forcefully disable PWMEN, true to release PWMEN control
   */
  void set_pwmen_pin( const bool enable );

  /**
   * @brief Sets the switching frequency of the LTC7871.
   *
   * This injects a square wave signal into the SYNC pin of the LTC7871 to
   * set the switching frequency of the power conversion hardware.
   *
   * @param frequency Desired frequency in kHz
   */
  void set_switching_frequency( const float frequency );

  /**
   * @brief Computes a new value for the MFR_IDAC_VLOW register to achieve vref.
   *
   * @param vlow  Desired output voltage
   * @param ra    Bottom resistor in VLow feedback divider
   * @param rb    Top resistor in VLow feedback divider
   * @return A valid IDAC_VLOW register value, or LTC_IDAC_REG_INVALID if invalid
   */
  uint8_t compute_idac_vlow( const float vlow, const float ra, const float rb );

  /**
   * @brief Computes a MFR_IDAC_SETCUR register value for max current limit
   *
   * @param ilim_gain Gain constant based on ILIM setting (pg. 16)
   * @param current Desired max current (A)
   * @param dcr     Phase inductor DCR (Ohms)
   * @return A valid IDAC_SETCUR register value, or LTC_IDAC_REG_INVALID if invalid
   */
  uint8_t compute_idac_setcur( const float ilim_gain, const float current, const float dcr );

  /**
   * @brief Check if the minimum on-time requirement is satisfied.
   *
   * See page 29 of the datasheet
   *
   * @param vout Output voltage of the LTC7871
   * @param vin  Input voltage of the LTC7871
   * @return True if the requirement is satisfied, false otherwise
   */
  bool min_on_time_satisfied( const float vout, const float vin );

}    // namespace HW::LTC7871::Private

#endif /* !ICHNAEA_LTC7871_PRIVATE_HPP */
