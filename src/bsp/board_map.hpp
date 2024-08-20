/******************************************************************************
 *  File Name:
 *    board_map.hpp
 *
 *  Description:
 *    Selects the appropriate board IO map for the current hardware.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_BOARD_MAP_HPP
#define ICHNAEA_BOARD_MAP_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/resets.h"
#include "hardware/spi.h"
#include "hardware/uart.h"
#include "pico/bootrom.h"
#include "pico/platform.h"
#include "pico/stdlib.h"
#include "pico/sync.h"
#include "pico/time.h"
#include <cstddef>
#include <cstdint>
#include <etl/array.h>

#include <mbedutils/assert.hpp>
#include <mbedutils/drivers/hardware/utility.hpp>

#if defined( PICO_RP2040 ) && ( PICO_RP2040 == 1 )
#define ICHNAEA_EMBEDDED
#else
#define ICHNAEA_SIMULATOR
#endif /* PICO_RP2040 */

namespace BSP
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  namespace Internal
  {
    /**
     * @brief LED pins to use in the event the board version cannot be determined.
     *
     * These must be defined here because the board version is not known at compile
     * time. Ideally these pins do not change in future hardware revisions.
     */
    static constexpr size_t UNMAPPED_LED_PINS[] = { 18, 19, 20, 21 };
    static constexpr size_t UNMAPPED_LED_NUM    = sizeof( UNMAPPED_LED_PINS ) / sizeof( UNMAPPED_LED_PINS[ 0 ] );
  }    // namespace Internal

  /*---------------------------------------------------------------------------
  Enumerations
  ---------------------------------------------------------------------------*/

  enum ADCPorts : size_t
  {
    ADC_MUTLIPLEXED_SENSE,

    ADC_MAX_PORTS
  };

  /**
   * @brief Available GPIO pins on the Ichnaea hardware
   *
   * These are only pins dedicated to logical GPIO functions. They do not
   * include alternate function pins dedicated for other purposes like
   * UART, SPI, etc.
   */
  enum GPIOPorts : size_t
  {
    GPIO_LTC_DCM,
    GPIO_LTC_CCM,
    GPIO_LTC_RUN,
    GPIO_LTC_ADCSEL0,
    GPIO_LTC_ADCSEL1,
    GPIO_LTC_ADCSEL2,
    GPIO_SPI_CS0,

    GPIO_MAX_PORTS
  };

  enum PWMPorts : size_t
  {
    PWM_LTC_SYNC,
    PWM_FAN_CONTROL,
    PWM_FAN_SENSE,
    PWM_LED_STATUS_0,
    PWM_LED_STATUS_1,
    PWM_LED_STATUS_2,
    PWM_LED_STATUS_3,

    PWM_MAX_PORTS
  };

  enum SPIPorts : size_t
  {
    SPI_LTC7871,

    SPI_MAX_PORTS
  };

  enum UARTPorts : size_t
  {
    UART_DEBUG,
    UART_BMS,

    UART_MAX_PORTS
  };

  /*---------------------------------------------------------------------------
  Public Types
  ---------------------------------------------------------------------------*/

  /**
   * @brief Abstracts the IO map for a given hardware version
   */
  struct IOConfig
  {
    size_t majorVersion;
    size_t minorVersion;

    struct ADC
    {
      size_t pin; /**< Pin selected for ADC input */
    };
    etl::array<ADC, ADC_MAX_PORTS> adc;

    struct GPIO
    {
      size_t pin; /**< Pin selected for output/input */
    };
    etl::array<GPIO, GPIO_MAX_PORTS> gpio;

    struct PWM
    {
      size_t pin; /**< Pin selected for PWM output */
    };
    etl::array<PWM, PWM_MAX_PORTS> pwm;

    struct SPI
    {
      size_t      sck;  /**< Pin selected for SCK */
      size_t      mosi; /**< Pin selected for MOSI */
      size_t      miso; /**< Pin selected for MISO */
      spi_inst_t *pHw;  /**< Reference to the HW register/instance */
    };
    etl::array<SPI, SPI_MAX_PORTS> spi;

    struct UART
    {
      size_t       tx;  /**< Pin selected for TX output */
      size_t       rx;  /**< Pin selected for RX input */
      uart_inst_t *pHw; /**< Reference to the HW register/instance */
    };
    etl::array<UART, UART_MAX_PORTS> uart;
  };

  /*-----------------------------------------------------------------------------
  Public Functions
  -----------------------------------------------------------------------------*/

  /**
   * @brief Inspects hardware to determine the appropriate IO map.
   *
   * Must be called before any other driver initialization.
   */
  void powerUp();

  /**
   * @brief Returns the IO map for the current hardware
   * @return const IOConfig&
   */
  const IOConfig &getIOConfig();

  /**
   * @brief Gets the pin associated with the given peripheral and logical name.
   *
   * Note that this only works for IO with a single pin. Multi-pin peripherals
   * will need to access the IOConfig directly.
   *
   * @param type Peripheral type to get the pin for
   * @param port Logical name of the pin, from one of the enums above
   * @return uint
   */
  uint getPin( const mb::hw::Peripheral type, const size_t port );

  /**
   * @brief Gets the hardware instance associated with the given peripheral and logical name
   *
   * @param type Peripheral type to get the pin for
   * @param port Logical name of the pin, from one of the enums above
   * @return void*
   */
  void *getHardware( const mb::hw::Peripheral type, const size_t port );
}    // namespace BSP

#endif /* !ICHNAEA_BOARD_MAP_HPP */
