/******************************************************************************
 *  File Name:
 *    led.cpp
 *
 *  Description:
 *    LED driver implementation
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <etl/algorithm.h>
#include <etl/array.h>
#include <src/bsp/board_map.hpp>
#include <src/hw/led.hpp>
#include <mbedutils/interfaces/time_intf.hpp>

namespace HW::LED
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  static constexpr float    POST_RAMP_TIME_MS  = 250.0f; /**< How long the POST ramp (Up then Dn) per LED should last */
  static constexpr float    POST_RAMP_STEPS    = 50.0f;  /**< Total brightness steps to take per ramp direction */
  static constexpr float    POST_RAMP_STEP_SZ  = 1.0f / POST_RAMP_STEPS;
  static constexpr uint32_t POST_RAMP_SLEEP_US = static_cast<uint32_t>( 1000.0f * ( 0.5f * POST_RAMP_TIME_MS / POST_RAMP_STEPS ) );
  static constexpr uint16_t PWM_COUNTER_WRAP   = 1000; /**< Arbitrary value to overflow the timer at */

  /*---------------------------------------------------------------------------
  Structures
  ---------------------------------------------------------------------------*/

  struct LEDState
  {
    uint     pin;         /**< Which pin this is physically mapped to */
    uint     pwm_slice;   /**< Which PWM slice is controlling this LED */
    uint     pwm_channel; /**< Which PWM channel is controlling this LED */
    uint16_t on_level;    /**< PWM level when ON */
    uint16_t off_level;   /**< PWM level when OFF */
    bool     enabled;     /**< Is the LED currently enabled */
  };

  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static etl::array<LEDState, Channel::NUM_OPTIONS> s_led_map;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    /*-------------------------------------------------------------------------
    Initialize the LED mapping
    -------------------------------------------------------------------------*/
    memset( &s_led_map, 0, sizeof( s_led_map ) );

    s_led_map[ STATUS_0 ].pin = BSP::getPin( mb::hw::PERIPH_PWM, BSP::PWM_LED_STATUS_0 );
    s_led_map[ STATUS_1 ].pin = BSP::getPin( mb::hw::PERIPH_PWM, BSP::PWM_LED_STATUS_1 );
    s_led_map[ STATUS_2 ].pin = BSP::getPin( mb::hw::PERIPH_PWM, BSP::PWM_LED_STATUS_2 );
    s_led_map[ STATUS_3 ].pin = BSP::getPin( mb::hw::PERIPH_PWM, BSP::PWM_LED_STATUS_3 );

    /*-------------------------------------------------------------------------
    Figure out the correct clock divider to get roughly 1kHz PWM frequency
    -------------------------------------------------------------------------*/
    const float f_clk_peri = static_cast<float>( frequency_count_khz( CLOCKS_FC0_SRC_VALUE_CLK_PERI ) );
    const float f_pwm      = 1'000.0f;
    const float divisor    = f_clk_peri / f_pwm;

    /*-------------------------------------------------------------------------
    Initialize the GPIO pins for PWM control
    -------------------------------------------------------------------------*/
    for( auto &state : s_led_map )
    {
      /*-----------------------------------------------------------------------
      Determine the PWM slice and channel for the given GPIO pin
      -----------------------------------------------------------------------*/
      state.pwm_slice   = pwm_gpio_to_slice_num( state.pin );
      state.pwm_channel = pwm_gpio_to_channel( state.pin );

      switch( BSP::getBoardRevision() )
      {
        case 1:
          state.off_level = PWM_COUNTER_WRAP + 1;
          break;

        case 2:
        default:
          state.off_level = 0;
          break;
      }

      state.on_level  = state.off_level;

      /*-----------------------------------------------------------------------
      Configure the GPIO pin for PWM output with pullups. The LEDs can ghost
      a bit if the pullups are not enabled.
      -----------------------------------------------------------------------*/
      gpio_set_function( state.pin, GPIO_FUNC_PWM );
      gpio_set_pulls( state.pin, false, false );

      /*-----------------------------------------------------------------------
      Initialize the PWM output to drive the LED off first. Inverted logic!
      -----------------------------------------------------------------------*/
      pwm_set_wrap( state.pwm_slice, PWM_COUNTER_WRAP );
      pwm_set_chan_level( state.pwm_slice, state.pwm_channel, state.off_level );
      pwm_set_clkdiv( state.pwm_slice, divisor );
      pwm_set_counter( state.pwm_slice, 0 );
    }

    /*-------------------------------------------------------------------------
    Enable each timer slice once configuration is complete so we don't get
    weird artifacts during initialization.
    -------------------------------------------------------------------------*/
    for( auto &state : s_led_map )
    {
      pwm_set_enabled( state.pwm_slice, true );
    }
  }


  void postSequence()
  {
    /*-------------------------------------------------------------------------
    Quickly pulse each led on a ramp up/down sequence
    -------------------------------------------------------------------------*/
    for( size_t channel = 0; channel < s_led_map.size(); channel++ )
    {
      float current_brightness = 0.0f;

      enable( channel );
      setBrightness( channel, current_brightness );

      for( uint32_t i = 0; i < POST_RAMP_STEPS; i++ )
      {
        current_brightness += POST_RAMP_STEP_SZ;
        setBrightness( channel, current_brightness );
        mb::time::delayMicroseconds( POST_RAMP_SLEEP_US );
      }

      for( uint32_t i = 0; i < POST_RAMP_STEPS; i++ )
      {
        current_brightness -= POST_RAMP_STEP_SZ;
        setBrightness( channel, current_brightness );
        mb::time::delayMicroseconds( POST_RAMP_SLEEP_US );
      }

      setBrightness( channel, 0.0f );
      disable( channel );
    }
  }


  void enable( const uint channel )
  {
    if( channel >= s_led_map.size() )
    {
      return;
    }

    auto &state = s_led_map[ channel ];
    pwm_set_chan_level( state.pwm_slice, state.pwm_channel, state.on_level );
    state.enabled = true;
  }


  void disable( const uint channel )
  {
    if( channel >= s_led_map.size() )
    {
      return;
    }

    auto &state = s_led_map[ channel ];
    pwm_set_chan_level( state.pwm_slice, state.pwm_channel, state.off_level );
    state.enabled = true;
  }


  void toggle( const uint channel )
  {
    if( channel >= s_led_map.size() )
    {
      return;
    }

    auto &state   = s_led_map[ channel ];
    state.enabled = !state.enabled;

    if( state.enabled )
    {
      pwm_set_chan_level( state.pwm_slice, state.pwm_channel, state.on_level );
    }
    else
    {
      pwm_set_chan_level( state.pwm_slice, state.pwm_channel, state.off_level );
    }
  }


  void setBrightness( const uint channel, const float brightness )
  {
    if( channel >= s_led_map.size() )
    {
      return;
    }

    /*-------------------------------------------------------------------------
    Clamp the brightness level to a valid range and convert it to a PWM level.
    -------------------------------------------------------------------------*/
    const float    clamped_brightness = etl::max( 0.0f, etl::min( brightness, 0.99f ) );
    uint16_t       approx_level       = etl::min( PWM_COUNTER_WRAP, static_cast<uint16_t>( PWM_COUNTER_WRAP * clamped_brightness ) );

    /*-------------------------------------------------------------------------
    Invert the brightness level if the board is version 1. Direct drive LEDs.
    -------------------------------------------------------------------------*/
    if( BSP::getIOConfig().majorVersion == 1 )
    {
      approx_level = PWM_COUNTER_WRAP - approx_level;
    }

    /*-------------------------------------------------------------------------
    Apply the updates to the cache to track for future state changes and then
    update the hardware.
    -------------------------------------------------------------------------*/
    auto &state = s_led_map[ channel ];
    state.on_level = approx_level;

    if( state.enabled )
    {
      pwm_set_chan_level( state.pwm_slice, state.pwm_channel, state.on_level );
    }
  }

}    // namespace HW::LED
