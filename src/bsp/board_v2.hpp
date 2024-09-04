/******************************************************************************
 *  File Name:
 *    board_v2.hpp
 *
 *  Description:
 *    Ichnaea v2 board specific IO map
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_BOARD_MAP_V2_HPP
#define ICHNAEA_BOARD_MAP_V2_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>
#include <cstddef>

namespace BSPV2
{
  static constexpr size_t BOARD_VERSION_MAJOR = 2;
  static constexpr size_t BOARD_VERSION_MINOR = 0;

  namespace ADC
  {
    static constexpr size_t PIN_IMON_FLTR = 26; // Channel 1
    static constexpr size_t PIN_MULTIPLEX = 27; // Channel 2
    static constexpr size_t PIN_BOARD_REV = 28; // Channel 3
    static constexpr size_t PIN_IMON_BATT = 29; // Channel 4
  }    // namespace ADC

  namespace GPIO
  {
    static constexpr size_t PIN_OUT_LTC_PWMEN = 11;
    static constexpr size_t PIN_OUT_LTC_DCM   = 9;
    static constexpr size_t PIN_OUT_LTC_CCM   = 8;
    static constexpr size_t PIN_OUT_LTC_RUN   = 7;
    static constexpr size_t PIN_OUT_ADC_SEL_0 = 6;
    static constexpr size_t PIN_OUT_ADC_SEL_1 = 5;
    static constexpr size_t PIN_OUT_ADC_SEL_2 = 4;
  }    // namespace GPIO

  namespace PWM
  {
    static constexpr size_t PIN_OUT_LTC_SYNC = 10;

    static constexpr size_t PIN_OUT_FAN_CTL  = 24;
    static constexpr size_t PIN_IN_FAN_SENSE = 25;

    static constexpr size_t PIN_OUT_LED_STATUS_0 = 18;
    static constexpr size_t PIN_OUT_LED_STATUS_1 = 19;
    static constexpr size_t PIN_OUT_LED_STATUS_2 = 22;
    static constexpr size_t PIN_OUT_LED_STATUS_3 = 23;
  }    // namespace PWM

  namespace SPI
  {
    /* SPI0 */
    static constexpr size_t PIN_NOR_SCK  = 2;
    static constexpr size_t PIN_NOR_MOSI = 3;
    static constexpr size_t PIN_NOR_MISO = 0;
    static constexpr size_t PIN_NOR_CS_0 = 1;

    /* SPI1 */
    static constexpr size_t PIN_LTC_SCK  = 14;
    static constexpr size_t PIN_LTC_MOSI = 15;
    static constexpr size_t PIN_LTC_MISO = 12;
    static constexpr size_t PIN_LTC_CS_0 = 13;
  }    // namespace SPI

  namespace UART
  {
    /* UART0 */
    static constexpr size_t PIN_DEBUG_TX = 16;
    static constexpr size_t PIN_DEBUG_RX = 17;

    /* UART1 */
    static constexpr size_t PIN_BMS_TX = 20;
    static constexpr size_t PIN_BMS_RX = 21;
  }    // namespace UART
}    // namespace BSPV2

#endif /* !ICHNAEA_BOARD_MAP_V2_HPP */
