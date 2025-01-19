/******************************************************************************
 *  File Name:
 *    fan.cpp
 *
 *  Description:
 *    Ichnaea fan control driver implementation
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>
#include <etl/algorithm.h>
#include <mbedutils/logging.hpp>
#include <mbedutils/threading.hpp>
#include <src/bsp/board_map.hpp>
#include <src/hw/fan.hpp>

namespace HW::FAN
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  static constexpr uint16_t PWM_COUNTER_WRAP = 1000; /**< Arbitrary value to overflow the timer at */
  static constexpr uint16_t PWM_COUNTER_OFF  = PWM_COUNTER_WRAP + 1;

  /*---------------------------------------------------------------------------
  Structures
  ---------------------------------------------------------------------------*/

  struct FanState
  {
    bool              enabled;          /**< Fan control enabled state */
    uint              pin_ctrl;         /**< PWM control pin */
    uint              pin_tach;         /**< Tachometer input pin */
    uint              ctrl_pwm_slice;   /**< PWM slice for control */
    uint              ctrl_pwm_channel; /**< PWM channel for control */
    uint16_t          ctrl_level;       /**< Current PWM level for control */
    volatile uint32_t tach_count;       /**< Tachometer pulse count */
    volatile float    tach_rpm;         /**< Tachometer RPM value */
    absolute_time_t   last_tach_time;   /**< Last time the tachometer was updated */
  };

  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static FanState s_fan;

  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Interrupt handler for the fan tachometer input
   *
   * @param gpio    (Unused) The GPIO pin that triggered the interrupt
   * @param events  (Unused) The type of event that triggered the interrupt
   */
  static void gpio_callback( uint gpio, uint32_t events )
  {
    ( void )gpio;
    ( void )events;

    s_fan.tach_count = s_fan.tach_count + 1u;

    // Calculate RPM every 2 seconds
    if( s_fan.tach_count >= 4 )
    {    // 2 pulses per revolution, so 4 pulses = 2 revolutions
      absolute_time_t current_time = get_absolute_time();
      uint32_t        time_diff    = absolute_time_diff_us( s_fan.last_tach_time, current_time );

      // Calculate RPM: (s_fan.tach_count / 2) * (60 seconds / time_diff in minutes)
      s_fan.tach_rpm       = ( s_fan.tach_count / 2.0f ) * ( 60.0f * 1000000.0f / time_diff );
      s_fan.tach_count     = 0;
      s_fan.last_tach_time = current_time;
    }
  }

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    /*-------------------------------------------------------------------------
    Initialize the fan state information
    -------------------------------------------------------------------------*/
    memset( &s_fan, 0, sizeof( FanState ) );

    s_fan.pin_ctrl       = BSP::getPin( mb::hw::PERIPH_PWM, BSP::PWM_FAN_CONTROL );
    s_fan.pin_tach       = BSP::getPin( mb::hw::PERIPH_PWM, BSP::PWM_FAN_SENSE );
    s_fan.enabled        = true;
    s_fan.last_tach_time = get_absolute_time();

    /*-------------------------------------------------------------------------
    Figure out the correct clock divider to get roughly 1kHz PWM frequency
    -------------------------------------------------------------------------*/
    const float f_clk_peri = static_cast<float>( frequency_count_khz( CLOCKS_FC0_SRC_VALUE_CLK_PERI ) );
    const float f_pwm      = 1000.0f;
    const float divisor    = f_clk_peri / f_pwm;

    /*-------------------------------------------------------------------------
    Initialize the GPIO pins for PWM control
    -------------------------------------------------------------------------*/
    s_fan.ctrl_pwm_slice   = pwm_gpio_to_slice_num( s_fan.pin_ctrl );
    s_fan.ctrl_pwm_channel = pwm_gpio_to_channel( s_fan.pin_ctrl );

    gpio_set_function( s_fan.pin_ctrl, GPIO_FUNC_PWM );
    gpio_set_pulls( s_fan.pin_ctrl, false, true );

    /*-------------------------------------------------------------------------
    Configure the PWM slice for fan control
    -------------------------------------------------------------------------*/
    pwm_set_wrap( s_fan.ctrl_pwm_slice, PWM_COUNTER_WRAP );
    pwm_set_chan_level( s_fan.ctrl_pwm_slice, s_fan.ctrl_pwm_channel, PWM_COUNTER_OFF );
    pwm_set_clkdiv( s_fan.ctrl_pwm_slice, divisor );
    pwm_set_counter( s_fan.ctrl_pwm_slice, 0 );
    pwm_set_enabled( s_fan.ctrl_pwm_slice, true );

    /*-------------------------------------------------------------------------
    Configure GPIO to trigger an interrupt on rising edge of the tach signal
    -------------------------------------------------------------------------*/
    gpio_set_function( s_fan.pin_tach, GPIO_FUNC_SIO );
    gpio_set_dir( s_fan.pin_tach, GPIO_IN );
    gpio_pull_up( s_fan.pin_tach );
    gpio_set_irq_enabled_with_callback( s_fan.pin_tach, GPIO_IRQ_EDGE_RISE, true, &gpio_callback );
  }


  void postSequence()
  {
    setSpeedRPM( 0.25f );
    mb::thread::this_thread::sleep_for( 2000 );
    setSpeedRPM( 0.1f );
  }


  void setSpeedRPM( const float speed )
  {
    /*-------------------------------------------------------------------------
    Clamp the brightness level to a valid range and convert it to a PWM level.
    -------------------------------------------------------------------------*/
    const float    clamped_speed = etl::max( 0.0f, etl::min( speed, 0.99f ) );
    const uint16_t approx_level  = etl::min( PWM_COUNTER_WRAP, static_cast<uint16_t>( PWM_COUNTER_WRAP * clamped_speed ) );

    /*-------------------------------------------------------------------------
    Apply the updates
    -------------------------------------------------------------------------*/
    s_fan.ctrl_level = approx_level;
    if( s_fan.enabled )
    {
      pwm_set_chan_level( s_fan.ctrl_pwm_slice, s_fan.ctrl_pwm_channel, s_fan.ctrl_level );
      LOG_TRACE( "Set fan speed: %.2f%", clamped_speed * 100.0f );
    }
  }


  float getFanSpeed()
  {
    return s_fan.tach_rpm;
  }

}    // namespace HW::FAN
