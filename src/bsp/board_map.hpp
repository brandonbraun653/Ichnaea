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
#include <mbedutils/interfaces/gpio_intf.hpp>
#include <mbedutils/interfaces/spi_intf.hpp>

#if defined( PICO_RP2040 ) && ( PICO_RP2040 == 1 )
#define ICHNAEA_EMBEDDED
#else
#define ICHNAEA_SIMULATOR
#endif /* PICO_RP2040 */

namespace BSP
{
  /*---------------------------------------------------------------------------
  Enumerations
  ---------------------------------------------------------------------------*/

  enum ADCPorts : size_t
  {
    /*-------------------------------------------------------------------------
    Version 1+
    -------------------------------------------------------------------------*/
    ADC_MUTLIPLEXED_SENSE, /**< Analog switch */

    /*-------------------------------------------------------------------------
    Version 2+
    -------------------------------------------------------------------------*/
    ADC_BOARD_REV, /**< Board revision */
    ADC_IMON_FLTR, /**< Filtered inductor avg current from LTC7871 */
    ADC_IMON_BATT, /**< Battery charge current */

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
    /*-------------------------------------------------------------------------
    Version 1+
    -------------------------------------------------------------------------*/
    GPIO_LTC_ADCSEL0, /**< Multiplexed ADC channel select */
    GPIO_LTC_ADCSEL1, /**< Multiplexed ADC channel select */
    GPIO_LTC_ADCSEL2, /**< Multiplexed ADC channel select */
    GPIO_LTC_DCM,     /**< LTC7871 DCM mode */
    GPIO_LTC_CCM,     /**< LTC7871 CCM mode */
    GPIO_LTC_RUN,     /**< LTC7871 Run mode */
    GPIO_SPI_CS_LTC,  /**< SPI chip select for LTC7871 */

    /*-------------------------------------------------------------------------
    Version 2+
    -------------------------------------------------------------------------*/
    GPIO_LTC_PWMEN,  /**< LTC7871 PWM enable */
    GPIO_SPI_CS_NOR, /**< NOR flash SPI chip select */

    GPIO_MAX_PORTS
  };

  enum PWMPorts : size_t
  {
    /*-------------------------------------------------------------------------
    Version 1+
    -------------------------------------------------------------------------*/
    PWM_LTC_SYNC,     /**< Frequency synchronization for LTC7871 */
    PWM_FAN_CONTROL,  /**< Fan speed control */
    PWM_FAN_SENSE,    /**< Fan speed sense */
    PWM_LED_STATUS_0, /**< Output LED status */
    PWM_LED_STATUS_1, /**< Output LED status */
    PWM_LED_STATUS_2, /**< Output LED status */
    PWM_LED_STATUS_3, /**< Output LED status */

    PWM_MAX_PORTS
  };

  enum SPIPorts : size_t
  {
    /*-------------------------------------------------------------------------
    Version 1+
    -------------------------------------------------------------------------*/
    SPI_LTC7871, /**< LTC7871 Power Controller */

    /*-------------------------------------------------------------------------
    Version 2+
    -------------------------------------------------------------------------*/
    SPI_NOR_FLASH, /**< NOR Flash Memory */

    SPI_MAX_PORTS
  };

  enum UARTPorts : size_t
  {
    /*-------------------------------------------------------------------------
    Version 1+
    -------------------------------------------------------------------------*/
    UART_BMS, /**< Battery Management System Interface*/

    /*-------------------------------------------------------------------------
    Version 2+
    -------------------------------------------------------------------------*/
    UART_DEBUG, /**< Debug Interface */

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
    size_t majorVersion;            /**< HW major version */
    size_t minorVersion;            /**< HW minor version */
    float  vmon_3v3_vdiv_r1;        /**< Voltage divider R1 for 3.3V sense */
    float  vmon_3v3_vdiv_r2;        /**< Voltage divider R2 for 3.3V sense */
    float  vmon_5v0_vdiv_r1;        /**< Voltage divider R1 for 5.0V sense */
    float  vmon_5v0_vdiv_r2;        /**< Voltage divider R2 for 5.0V sense */
    float  vmon_12v_vdiv_r1;        /**< Voltage divider R1 for 12V sense */
    float  vmon_12v_vdiv_r2;        /**< Voltage divider R2 for 12V sense */
    float  vmon_load_vdiv_r1;       /**< Voltage divider R1 for load sense */
    float  vmon_load_vdiv_r2;       /**< Voltage divider R2 for load sense */
    float  imon_load_vdiv_r1;       /**< Voltage divider R1 for load current sense */
    float  imon_load_vdiv_r2;       /**< Voltage divider R2 for load current sense */
    float  imon_load_rsense;        /**< Current sense resistor for load current */
    float  imon_load_opamp_gain;    /**< Op-amp gain for load current sense */
    float  vmon_solar_vdiv_r1;      /**< Voltage divider R1 for solar sense */
    float  vmon_solar_vdiv_r2;      /**< Voltage divider R2 for solar sense */
    float  tmon_vdiv_input;         /**< Voltage divider input reference voltage */
    float  tmon_vdiv_r1_fixed;      /**< Voltage divider R1 for temperature sense */
    float  tmon_vdiv_r2_thermistor; /**< Voltage divider R2 for temperature sense */
    float  tmon_beta_25C;           /**< Beta value for the thermistor @25C */
    float  ltc_inductor_dcr;        /**< Inductor DC resistance for LTC7871 */
    float  ltc_vlow_ra;             /**< LTC7871 VLow feedback divider resistor A */
    float  ltc_vlow_rb;             /**< LTC7871 VLow feedback divider resistor B */
    size_t ltc_num_phases;          /**< Number of populated phases on the LTC7871 */
    float  ltc_setcur_rfb;          /**< SETCUR control resistor */

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
      size_t              sck;  /**< Pin selected for SCK */
      size_t              mosi; /**< Pin selected for MOSI */
      size_t              miso; /**< Pin selected for MISO */
      spi_inst_t         *pHw;  /**< Reference to the HW register/instance */
      mb::hw::spi::Port_t port; /**< Physical peripheral instance id */
    };
    etl::array<SPI, SPI_MAX_PORTS> spi;

    struct UART
    {
      size_t       tx;  /**< Pin selected for TX output */
      size_t       rx;  /**< Pin selected for RX input */
      uart_inst_t *pHw; /**< Reference to the HW register/instance */

#if defined( ICHNAEA_SIMULATOR )
      std::string zmq_endpoint; /**< ZMQ endpoint for the simulator */
#endif
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
   * @return unsigned int
   */
  unsigned int getPin( const mb::hw::Peripheral type, const size_t port );

  /**
   * @brief Gets the hardware instance associated with the given peripheral and logical name
   *
   * @param type Peripheral type to get the pin for
   * @param port Logical name of the pin, from one of the enums above
   * @return void*
   */
  void *getHardware( const mb::hw::Peripheral type, const size_t port );

  /**
   * @brief Get the current board revision
   * @return uint8_t
   */
  uint8_t getBoardRevision();

#if defined( ICHNAEA_SIMULATOR )
  /**
   * @brief Gets the ZMQ endpoint for the given device port
   *
   * @param type Peripheral type to get the endpoint for
   * @param port Port to get the endpoint for
   * @return std::string
   */
  std::string getZMQEndpoint( const mb::hw::Peripheral type, const size_t port );
#endif

}    // namespace BSP

#endif /* !ICHNAEA_BOARD_MAP_HPP */
