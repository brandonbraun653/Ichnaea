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
    POST_SEQUENCE,    /* Running the power on self test */
    INITIALIZING,     /* Booting up and checking for ability to operate */
    NORMAL_OPERATION, /* System running normally */
    FAULTED           /* Error state due to abnormal behavior */
  };

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initializes the LTC7871 driver
   */
  void initialize();

  /**
   * @brief Perform power on self test (POST) for the LTC7871
   * @warning This function has a dependency on the BMS communication bus.
   */
  void postSequence();

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

}  // namespace HW::LTC7871

#endif  /* !ICHNAEA_LTC7871_HPP */
