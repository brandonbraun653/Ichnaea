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
#include "src/hw/fan.hpp"
#include "src/bsp/board_map.hpp"
#include <mbedutils/logging.hpp>

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
    uint     pin_ctrl;
    uint     pin_tach;
    uint     ctrl_pwm_slice;
    uint     ctrl_pwm_channel;
    uint16_t ctrl_level;
    bool     enabled;
    uint tach_pwm_slice;
    uint tach_pwm_channel;
    volatile uint32_t tach_count;
    volatile float tach_rpm;
    absolute_time_t last_tach_time;
  };

  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static FanState s_fan;

  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/
  static void gpio_callback( uint gpio, uint32_t events )
  {
    s_fan.tach_count = s_fan.tach_count + 1u;

    // Calculate RPM every 2 seconds
    if( s_fan.tach_count >= 4 )
    {    // 2 pulses per revolution, so 4 pulses = 2 revolutions
      absolute_time_t current_time = get_absolute_time();
      uint32_t        time_diff    = absolute_time_diff_us( s_fan.last_tach_time, current_time );

      // Calculate RPM: (s_fan.tach_count / 2) * (60 seconds / time_diff in minutes)
      s_fan.tach_rpm = ( s_fan.tach_count / 2.0f ) * ( 60.0f * 1000000.0f / time_diff );

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
    s_fan.pin_ctrl = BSP::getPin( mb::hw::PERIPH_PWM, BSP::PWM_FAN_CONTROL );
    s_fan.pin_tach = BSP::getPin( mb::hw::PERIPH_PWM, BSP::PWM_FAN_SENSE );
    s_fan.enabled  = true;
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

    s_fan.tach_pwm_slice   = pwm_gpio_to_slice_num( s_fan.pin_tach );
    s_fan.tach_pwm_channel = pwm_gpio_to_channel( s_fan.pin_tach );

    gpio_set_function( s_fan.pin_tach, GPIO_FUNC_PWM );
    gpio_set_function( s_fan.pin_ctrl, GPIO_FUNC_PWM );
    gpio_set_pulls( s_fan.pin_ctrl, false, true );

    // PWM setup
    pwm_set_wrap( s_fan.ctrl_pwm_slice, PWM_COUNTER_WRAP );
    pwm_set_chan_level( s_fan.ctrl_pwm_slice, s_fan.ctrl_pwm_channel, PWM_COUNTER_OFF );
    pwm_set_clkdiv( s_fan.ctrl_pwm_slice, divisor );
    pwm_set_counter( s_fan.ctrl_pwm_slice, 0 );
    pwm_set_enabled( s_fan.ctrl_pwm_slice, true );

    // Tachometer input setup (pin 23)
    gpio_set_function(s_fan.pin_tach, GPIO_FUNC_SIO);
    gpio_set_dir(s_fan.pin_tach, GPIO_IN);
    gpio_pull_up(s_fan.pin_tach);

    gpio_set_irq_enabled_with_callback(s_fan.pin_tach, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    setSpeedPercent( 0.1f );
  }


  void postSequence()
  {
    LOG_INFO( "Fan RPM: %.2f", getFanSpeedRPM() );
  }


  void setSpeedPercent( const float speed )
  {
    /*-------------------------------------------------------------------------
    Clamp the brightness level to a valid range and convert it to a PWM level.
    -------------------------------------------------------------------------*/
    const float    clamped_speed  = MAX( 0.0f, MIN( speed, 0.99f ) );
    const uint16_t approx_level   = MIN( PWM_COUNTER_WRAP, static_cast<uint16_t>( PWM_COUNTER_WRAP * clamped_speed ) );

    /*-------------------------------------------------------------------------
    Apply the updates
    -------------------------------------------------------------------------*/
    s_fan.ctrl_level = approx_level;
    if( s_fan.enabled )
    {
      pwm_set_chan_level( s_fan.ctrl_pwm_slice, s_fan.ctrl_pwm_channel, s_fan.ctrl_level );
    }
  }


  float getFanSpeedRPM()
  {
    return s_fan.tach_rpm;
  }

}    // namespace HW::FAN
