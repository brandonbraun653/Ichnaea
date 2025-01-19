/******************************************************************************
 *  File Name:
 *    app_monitor.hpp
 *
 *  Description:
 *    Function declarations for system monitoring actions. These functions
 *    are intended to be called frequently but will only execute based on
 *    their PDI configuration.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_SYSTEM_MONITOR_HPP
#define ICHNAEA_SYSTEM_MONITOR_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <src/system/system_sensor.hpp>

namespace App::Monitor
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/
  /**
   * @brief Initialize the driver
   */
  void driver_init();

  /**
   * @brief Deinitialize the driver
   */
  void driver_deinit();

  /**
   * @brief Enable monitor operation
   */
  void enable();

  /**
   * @brief Disable monitor operation
   */
  void disable();

  /**
   * @brief Reset internal stae of all the monitors
   */
  void reset();

  /**
   * @brief Reload the static PDI dependencies for a given monitor
   *
   * @param element Which sensor monitor to refresh
   */
  void refreshPDIDependencies( const System::Sensor::Element element );

  /**
   * @brief Monitor input voltage
   */
  void monitorInputVoltage();

  /**
   * @brief Monitor output current
   */
  void monitorOutputCurrent();

  /**
   * @brief Monitor output voltage
   */
  void monitorOutputVoltage();

  /**
   * @brief Monitor 1.1V voltage
   */
  void monitor1V1Voltage();

  /**
   * @brief Monitor 3.3V voltage
   */
  void monitor3V3Voltage();

  /**
   * @brief Monitor 5.0V voltage
   */
  void monitor5V0Voltage();

  /**
   * @brief Monitor 12.0V voltage
   */
  void monitor12V0Voltage();

  /**
   * @brief Monitor temperature
   */
  void monitorTemperature();

  /**
   * @brief Monitor fan speed
   */
  void monitorFanSpeed();
}    // namespace App::Monitor

#endif /* !ICHNAEA_SYSTEM_MONITOR_HPP */
