/******************************************************************************
 *  File Name:
 *    ltc7871.hpp
 *
 *  Description:
 *    Ichnaea LTC7871 driver interface
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
   * @brief Specific reasons why the LTC7871 faulted
   */
  enum class FaultCode : size_t
  {
    NO_FAULT = 0,

    NUM_OPTIONS
  };


  /**
   * @brief Internal classification of modes the driver can be in.
   *
   * This is extremely helpful for error handling and command parsing to ensure
   * the driver is in a valid state to perform certain behaviors.
   */
  enum class DriverMode : size_t
  {
    DISABLED,         /* System is at an idle state an not operating */
    FAULTED,          /* Error state due to abnormal behavior */
    POST_SEQUENCE,    /* Running the power on self test */
    INITIALIZING,     /* Booting up and checking for ability to operate */
    NORMAL_OPERATION, /* System running normally */
  };


  /**
   * @brief Discrete voltage levels that can be set/detected on the ILIM pin.
   *
   * This controls the current comparator threshold for the LTC7871.
   */
  enum class ILim : size_t
  {
    Unknown, /**< ILIM pin voltage is unknown */
    V0,      /**< ILIM pin voltage is zero */
    V1_4,    /**< ILIM pin voltage is 1/4 V5 */
    V3_4,    /**< ILIM pin voltage is 3/4 V5 */
    V5       /**< ILIM pin voltage is V5 */
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


  enum LTCStatusBits : uint32_t
  {
    LTC_STATUS_PGOOD       = 0, /**< Regulated VLow/VHigh is within +/-10% */
    LTC_STATUS_MAX_CURRENT = 1, /**< Maximum current limit reached */
    LTC_STATUS_SS_DONE     = 2, /**< Soft-start sequence is complete */

    LTC_STATUS_COUNT
  };

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initializes the LTC7871 driver
   */
  void initialize();

  /**
   * @brief Status flag to indicate if it's safe to interact with the LTC7871.
   *
   * This will only be true if the LTC7871 is powered on, not in a faulted
   * state, and the driver has performed initialization and POST steps.
   *
   * @return true  The LTC7871 is ready for interaction
   * @return false The LTC7871 is not ready
   */
  bool available();

  /**
   * @brief Enables the LTC7871 and configures it for normal operation.
   *
   * If successful, this will start power flowing from the solar input
   * to the low voltage output. If unsuccessful, the LTC7871 will be
   * returned back to a safe state and the system will be shutdown.
   */
  void powerOn();

  /**
   * @brief Disables the LTC7871 and puts it into a low power state.
   *
   * This will cut off power conversion and prevent the LTC7871 from
   * interacting with the system until the power up sequence is run.
   */
  void powerOff();

  /**
   * @brief Single step the core controller of the LTC7871.
   *
   * This should be called periodically to ensure the LTC7871 is operating
   * correctly. It will handle all the necessary state transitions and
   * fault checking.
   */
  void stepController();

  /**
   * @brief Sets the system output voltage reference.
   *
   * Assuming the controller is in a state to accept this command, the
   * output voltage will ultimately be adjusted to the desired level.
   *
   * @param voltage New voltage level to target
   */
  void setVoutRef( const float voltage );

  /**
   * @brief Reads the fault registers of the LTC7871.
   *
   * This collapses the 3 fault registers into a single 32-bit value.
   *
   * @return A bit field of LTCFaultBits
   */
  uint32_t readLTCFaults();

  /**
   * @brief Reads the status register of the LTC7871.
   *
   * @return uint32_t
   */
  uint32_t readLTCStatus();

  /**
   * @brief Converts a fault code to a human readable string
   *
   * @param code  Fault code to convert
   * @return Loggable/printable error message of the code
   */
  const char *ltcFaultCodeToString( const uint32_t code );

  /**
   * @brief Gets the current operational mode of the driver
   *
   * @return DriverMode
   */
  DriverMode getMode();

  /**
   * @brief Gets the latest fault code from the driver
   *
   * @return FaultCode
   */
  FaultCode getFaultCode();

  /**
   * @brief Resets the fault code to NO_FAULT
   */
  void clearFaultCode();

  /**
   * @brief Gets the detected ILIM pin setting from HW strapping.
   *
   * @return ILim
   */
  ILim getILim();

  /**
   * @brief Gets the inductor current sense resistor value
   *
   * @return float
   */
  float getRSense();

}  // namespace HW::LTC7871

#endif  /* !ICHNAEA_LTC7871_HPP */
