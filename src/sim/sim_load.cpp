/******************************************************************************
 *  File Name:
 *    sim_load.cpp
 *
 *  Description:
 *    Load modeling for the simulation environment
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <src/bsp/board_map.hpp>
#include <src/hw/adc.hpp>
#include <src/sim/sim_load.hpp>
#include <src/sim/hw/sim_adc.hpp>
#include <src/system/system_sensor.hpp>
#include <mbedutils/drivers/hardware/analog.hpp>
#include <mutex>

namespace SIM::Load
{
  /*---------------------------------------------------------------------------
  Constants
  ---------------------------------------------------------------------------*/

  static constexpr float kMinLoadResistance = 1e-3f;    // Low impedance state
  static constexpr float kMaxLoadResistance = 1e9f;     // High impedance state

  /*---------------------------------------------------------------------------
  Private Data
  ---------------------------------------------------------------------------*/

  static std::mutex s_load_mutex;     /**< Thread safety */
  static float      s_resistive_load; /**< Current resistive load in ohms */

  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/

  static float update_adc_load_current_measurement()
  {
    /*-------------------------------------------------------------------------
    Calculate the current flowing through the load
    -------------------------------------------------------------------------*/
    float iout = 0.0f;
    {
      std::scoped_lock lock( s_load_mutex );
      iout = System::Sensor::getMeasurement( System::Sensor::Element::VMON_LOAD ) / s_resistive_load;
    }

    /*-------------------------------------------------------------------------
    Reverse the gain/voltage divider to get the input voltage to the ADC
    -------------------------------------------------------------------------*/
    auto ioConfig = BSP::getIOConfig();

    // Compute the voltage across the sense resistor
    float vsense = ( iout * ioConfig.imon_load_rsense ) * ioConfig.imon_load_opamp_gain;

    // Clamp the voltage to the op-amp limits
    std::clamp( vsense, 0.0f, 4.9f );

    // Compute the measured ADC input voltage
    float vadc = Analog::calculateVoltageDividerOutput( vsense, ioConfig.imon_load_vdiv_r1, ioConfig.imon_load_vdiv_r2 );

    return vadc;
  }

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    reset();
    HW::ADC::set_update_callback( HW::ADC::Channel::IMON_LOAD, update_adc_load_current_measurement );
  }


  void reset()
  {
    std::lock_guard lock( s_load_mutex );
    s_resistive_load = kMaxLoadResistance;
  }


  void setResistiveLoad( const float resistance )
  {
    std::lock_guard lock( s_load_mutex );
    s_resistive_load = std::clamp( resistance, kMinLoadResistance, kMaxLoadResistance );
  }

}    // namespace SIM::Load
