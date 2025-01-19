/******************************************************************************
 *  File Name:
 *    adc.cpp
 *
 *  Description:
 *    Ichnaea ADC driver implementation
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "mbedutils/drivers/hardware/analog.hpp"
#include "mbedutils/interfaces/time_intf.hpp"
#include <array>
#include <deque>
#include <mutex>
#include <src/hw/adc.hpp>
#include <src/bsp/board_map.hpp>
#include <src/sim/hw/sim_adc.hpp>

namespace HW::ADC
{
  /*---------------------------------------------------------------------------
  Structures
  ---------------------------------------------------------------------------*/

  struct ADCSample
  {
    float   voltage;   /**< Voltage measured by the ADC peripheral */
    int64_t timestamp; /**< System tick time (ms) when the sample is valid */

    ADCSample() : voltage( 0.0f ), timestamp( 0 )
    {
    }
  };

  using ADCSampleStream = std::deque<ADCSample>;

  struct ADCControlBlock
  {
    UpdateCallback  update_callback;    /**< Callback to update the ADC channel */
    ADCSampleStream sample_stream;      /**< Data stream for queuing samples to read back */
    float           last_known_voltage; /**< Last known voltage measurement */
  };


  /*---------------------------------------------------------------------------
  Private Data
  ---------------------------------------------------------------------------*/

  static std::recursive_mutex                              s_adc_mutex;    /**< Thread safety */
  static std::array<ADCControlBlock, Channel::NUM_OPTIONS> s_adc_channels; /**< ADC channel data */

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    /*-------------------------------------------------------------------------
    Reset the ADC channel data
    -------------------------------------------------------------------------*/
    for( auto &cb : s_adc_channels )
    {
      cb.last_known_voltage = 0.0f;
      cb.sample_stream.clear();
      cb.update_callback = nullptr;
    }

    /*-------------------------------------------------------------------------
    Set the nominal temperature to 25C
    -------------------------------------------------------------------------*/
    auto  ioConfig    = BSP::getIOConfig();
    float adc_reading = Analog::calculateVoltageDividerOutput( ioConfig.tmon_vdiv_input, ioConfig.tmon_vdiv_r1_fixed,
                                                               ioConfig.tmon_vdiv_r2_thermistor );
    inject_sample( Channel::TEMP_SENSE_0, adc_reading, mb::time::millis() );
    inject_sample( Channel::TEMP_SENSE_1, adc_reading, mb::time::millis() );
  }


  void postSequence()
  {
    // Nothing to do here for now
  }


  float getVoltage( const size_t channel )
  {
    /*-------------------------------------------------------------------------
    Input Protection
    -------------------------------------------------------------------------*/
    if( channel >= s_adc_channels.size() )
    {
      return 0.0f;
    }

    std::lock_guard lock( s_adc_mutex );

    /*-------------------------------------------------------------------------
    Invoke update callback for the given channel if it exists
    -------------------------------------------------------------------------*/
    if( s_adc_channels[ channel ].update_callback )
    {
      s_adc_channels[ channel ].last_known_voltage = s_adc_channels[ channel ].update_callback();
    }

    /*-------------------------------------------------------------------------
    Update the cache with any newly injected samples.
    -------------------------------------------------------------------------*/
    else if( !s_adc_channels[ channel ].sample_stream.empty() )
    {
      /*-----------------------------------------------------------------------
      Find the next sample that most closely matches the current time, but is
      not in the future.
      -----------------------------------------------------------------------*/

      // TODO BMB: Ultra lazy implementation right now. Need to update this for accurate
      // time based sampling with more complex waveforms. Going to depend on how quickly
      // the sampling function is called by the application.

      auto sample_data = s_adc_channels[ channel ].sample_stream.front();
      if( sample_data.timestamp >= mb::time::millis() )
      {
        return getCachedVoltage( channel );
      }

      /*-----------------------------------------------------------------------
      Consume the sample. Update the cache as the last known voltage.
      -----------------------------------------------------------------------*/
      s_adc_channels[ channel ].sample_stream.pop_front();
      s_adc_channels[ channel ].last_known_voltage = sample_data.voltage;
    }

    return getCachedVoltage( channel );
  }


  float getCachedVoltage( const size_t channel )
  {
    if( channel < s_adc_channels.size() )
    {
      std::lock_guard lock( s_adc_mutex );

      // TODO BMB: Could get real fancy here later on and inject some noise into the readings
      // to simulate real world conditions. For now, just return the last known value.
      return s_adc_channels[ channel ].last_known_voltage;
    }

    return 0.0f;
  }


  void inject_sample( const Channel channel, const float voltage, const size_t timestamp )
  {
    if( channel < s_adc_channels.size() )
    {
      std::lock_guard lock( s_adc_mutex );

      ADCSample sample;
      sample.voltage   = voltage;
      sample.timestamp = timestamp;

      s_adc_channels[ channel ].sample_stream.push_back( sample );
    }
  }


  void set_update_callback( const Channel channel, const UpdateCallback &callback )
  {
    if( channel < s_adc_channels.size() )
    {
      std::lock_guard lock( s_adc_mutex );
      s_adc_channels[ channel ].update_callback = callback;
    }
  }
}    // namespace HW::ADC
