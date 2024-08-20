/******************************************************************************
 *  File Name:
 *    board_v1.hpp
 *
 *  Description:
 *    Ichnaea v1 board specific IO map
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_BOARD_MAP_V1_HPP
#define ICHNAEA_BOARD_MAP_V1_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>
#include <cstddef>

namespace BSPV1
{
  static constexpr size_t BOARD_VERSION_MAJOR = 1;
  static constexpr size_t BOARD_VERSION_MINOR = 0;

  namespace ADC
  {
    static constexpr size_t PIN_SENSE = 26;
  }    // namespace ADC

  namespace GPIO
  {
    static constexpr size_t PIN_OUT_LTC_DCM      = 2;
    static constexpr size_t PIN_OUT_LTC_CCM      = 3;
    static constexpr size_t PIN_IN_LTC_SLAVE     = 24;
    static constexpr size_t PIN_IO_LTC_RUN       = 25;
    static constexpr size_t PIN_OUT_ADC_SEL_0    = 5;
    static constexpr size_t PIN_OUT_ADC_SEL_1    = 6;
    static constexpr size_t PIN_OUT_ADC_SEL_2    = 7;
    static constexpr size_t PIN_OUT_LED_STATUS_0 = 18;
    static constexpr size_t PIN_OUT_LED_STATUS_1 = 19;
    static constexpr size_t PIN_OUT_LED_STATUS_2 = 20;
    static constexpr size_t PIN_OUT_LED_STATUS_3 = 21;
  }    // namespace GPIO

  namespace I2C
  {
    static constexpr size_t PIN_SDA = 0;
    static constexpr size_t PIN_SCK = 1;
  }    // namespace I2C

  namespace PWM
  {
    static constexpr size_t PIN_OUT_LTC_SYNC = 4;
    static constexpr size_t PIN_OUT_FAN_CTL  = 22;
    static constexpr size_t PIN_IN_FAN_SENSE = 23;
  }    // namespace PWM

  namespace SPI
  {
    static constexpr size_t PIN_SCK  = 10;
    static constexpr size_t PIN_MOSI = 11;
    static constexpr size_t PIN_MISO = 12;
    static constexpr size_t PIN_CS_0 = 13;
  }    // namespace SPI

  namespace UART
  {
    static constexpr size_t PIN_TX = 16;
    static constexpr size_t PIN_RX = 17;
  }    // namespace UART
}    // namespace BSPV1

#endif /* !ICHNAEA_BOARD_MAP_V1_HPP */
