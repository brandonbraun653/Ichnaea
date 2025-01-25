/******************************************************************************
 *  File Name:
 *    ltc7871.hpp
 *
 *  Description:
 *    Ichnaea LTC7871 driver interface. This layer is intended to provide
 *    high level direct control of the power converter. Minimal safety checks
 *    are in place as most system level guardrails are done the the application
 *    layer. Use with caution.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_LTC7871_HPP
#define ICHNAEA_LTC7871_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>
#include <cstddef>

namespace HW::LTC7871
{
  /*---------------------------------------------------------------------------
  Enumerations
  ---------------------------------------------------------------------------*/

  /**
   * @brief Internal classification of modes the driver can be in.
   *
   * This is extremely helpful for error handling and command parsing to ensure
   * the driver is in a valid state to perform certain behaviors.
   */
  enum class DriverMode : size_t
  {
    DISABLED, /* System is at an idle state an not operating */
    FAULTED,  /* Error state due to abnormal behavior */
    ENABLED,  /* System running normally */
  };

  /**
   * @brief Available fault status codes for the LTC7871
   */
  enum LTCFaultBits : uint32_t
  {
    // MFR_FAULT Register
    LTC_FAULT_OVERTEMP = 0, /**< An overtemperature fault has occurred */
    LTC_FAULT_VREF_BAD = 1, /**< The internal reference self-check failed */
    LTC_FAULT_V5_UV    = 2, /**< The V5 pin is undervoltage */
    LTC_FAULT_DRVCC_UV = 3, /**< The DRVcc pin is undervoltage */
    LTC_FAULT_VHIGH_UV = 4, /**< The UVHigh pin is less than 1.2v threshold */
    LTC_FAULT_VHIGH_OV = 5, /**< The UVHigh pin is greater than 1.2v threshold */
    LTC_FAULT_VLOW_OV  = 6, /**< The UVLow pin is greater than 1.2v threshold */

    // MFR_OC_FAULT Register
    LTC_FAULT_OC_1 = 7,  /**< Channel 1 overcurrent fault has occurred */
    LTC_FAULT_OC_2 = 8,  /**< Channel 2 overcurrent fault has occurred */
    LTC_FAULT_OC_3 = 9,  /**< Channel 3 overcurrent fault has occurred */
    LTC_FAULT_OC_4 = 10, /**< Channel 4 overcurrent fault has occurred */
    LTC_FAULT_OC_5 = 11, /**< Channel 5 overcurrent fault has occurred */
    LTC_FAULT_OC_6 = 12, /**< Channel 6 overcurrent fault has occurred */

    // MFR_NOC_FAULT Register
    LTC_FAULT_NOC_1 = 13, /**< Channel 1 negative overcurrent fault has occurred */
    LTC_FAULT_NOC_2 = 14, /**< Channel 2 negative overcurrent fault has occurred */
    LTC_FAULT_NOC_3 = 15, /**< Channel 3 negative overcurrent fault has occurred */
    LTC_FAULT_NOC_4 = 16, /**< Channel 4 negative overcurrent fault has occurred */
    LTC_FAULT_NOC_5 = 17, /**< Channel 5 negative overcurrent fault has occurred */
    LTC_FAULT_NOC_6 = 18, /**< Channel 6 negative overcurrent fault has occurred */

    LTC_FAULT_COUNT
  };

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initializes the LTC7871 driver
   */
  void driver_init();

  /**
   * @brief Deinitializes the LTC7871 driver
   */
  void driver_deinit();

  /**
   * @brief POST sequence for the LTC7871
   */
  void postSequence();

  /**
   * @brief Gets the current operational mode of the driver
   *
   * @return DriverMode
   */
  DriverMode getMode();

  /**
   * @brief Reads the fault registers of the LTC7871.
   *
   * This collapses the 3 fault registers into a single 32-bit value with the
   * faulted bit positions corresponding to the LTCFaultBits enumeration.
   *
   * @return A bit field of LTCFaultBits
   */
  uint32_t getFaults();

  /**
   * @brief Resets LTC7871 fault status bits
   */
  void clearFaults();

  /**
   * @brief Enables the LTC7871 power stage output.
   *
   * This will go through a series of checks to make sure the system is in a safe
   * state to enable the power converter. If any of the checks fail, the system
   * will be placed into a faulted state and the power converter will not be enabled.
   *
   * @param vout Desired output voltage (Volts)
   * @param iout Desired output current limit (Amps)
   * @return True if the power converter was successfully enabled, false otherwise
   */
  bool enablePowerConverter( const float vout, const float iout );

  /**
   * @brief Disables the LTC7871 power stage output.
   */
  void disablePowerConverter();

  /**
   * @brief Run background processing to keep the LTC state up to date
   */
  void runStateUpdater();

  /**
   * @brief Sets the system output voltage reference.
   *
   * Assuming the controller is in a state to accept this command, the
   * output voltage will ultimately be adjusted to the desired level.
   *
   * @param voltage New voltage level to target (Volts)
   */
  void setVoutRef( const float voltage );

  /**
   * @brief Sets the system output current reference.
   *
   * Assuming the controller is in a state to accept this command, the
   * output current limit will be adjusted to the desired level.
   *
   * @param current New current level to target (Amps)
   */
  void setIoutRef( const float current );

  /**
   * @brief Gets the reported average output current from the LTC7871.
   *
   * @param voltage Measured voltage from the IMON pin (Volts)
   * @return float Average output current (Amps)
   */
  float getAverageOutputCurrent( float voltage );

  /**
   * @brief Monitors the LTC7871 for faults and transitions the system state as needed.
   */
  void runFaultMonitoring();

}    // namespace HW::LTC7871

#endif /* !ICHNAEA_LTC7871_HPP */
