/******************************************************************************
 *  File Name:
 *    sim_ltc7871.cpp
 *
 *  Description:
 *    Idealized simulator implementation of the LTC7871 power buck converter.
 *    This models the logical behavior of the LTC7871, but does not interact
 *    with any physical hardware.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>
#include <mbedutils/drivers/hardware/analog.hpp>
#include <mbedutils/logging.hpp>
#include <src/bsp/board_map.hpp>
#include <src/hw/adc.hpp>
#include <src/hw/ltc7871.hpp>
#include <src/hw/ltc7871_prv.hpp>
#include <src/hw/ltc7871_reg.hpp>
#include <src/sim/hw/sim_adc.hpp>
#include <src/sim/sim_load.hpp>

namespace HW::LTC7871
{
  /*---------------------------------------------------------------------------
  Private Data
  ---------------------------------------------------------------------------*/

  static bool       s_ltc7871_enabled;
  static uint32_t   s_ltc7871_faults;
  static float      s_vout_ref;
  static float      s_iout_ref;
  static float      s_phase_iout_ref;
  static DriverMode s_driver_mode;

  /*---------------------------------------------------------------------------
  Private Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Computes the analog voltage on the LTC average current output pin.
   * @return float
   */
  static float adc_ltc_imon_update_callback()
  {
    const float imon_vnom = 1.25f;

    // TODO: Simulate the current monitor if we end up needing it.
    // float current = SIM::Load::currentNow();

    // Output voltage is range limited (pg. 16 of datasheet)
    return std::clamp( imon_vnom, 0.4f, 2.5f );
    ;
  }

  /**
   * @brief Computes the ADC measurement of the LTC output voltage.
   * @return float
   */
  static float adc_ltc_vout_update_callback()
  {
    auto ioConfig = BSP::getIOConfig();
    return Analog::calculateVoltageDividerOutput( s_vout_ref, ioConfig.vmon_load_vdiv_r1, ioConfig.vmon_load_vdiv_r2 );
  }

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void driver_init()
  {
    /*---------------------------------------------------------------------------
    Initialize module data
    ---------------------------------------------------------------------------*/
    s_ltc7871_enabled = false;
    s_ltc7871_faults  = 0;
    s_vout_ref        = 0.0f;
    s_iout_ref        = 0.0f;
    s_phase_iout_ref  = 0.0f;
    s_driver_mode     = DriverMode::DISABLED;

    /*-------------------------------------------------------------------------
    Register ADC measurement callbacks
    -------------------------------------------------------------------------*/
    HW::ADC::set_update_callback( HW::ADC::Channel::LTC_IMON, adc_ltc_imon_update_callback );
    HW::ADC::set_update_callback( HW::ADC::Channel::LV_DC_SENSE, adc_ltc_vout_update_callback );
  }


  void driver_deinit()
  {
    HW::ADC::set_update_callback( HW::ADC::Channel::LTC_IMON, nullptr );
    HW::ADC::set_update_callback( HW::ADC::Channel::LV_DC_SENSE, nullptr );
  }


  void postSequence()
  {
  }


  bool enablePowerConverter( const float vout, const float iout )
  {
    s_ltc7871_enabled = true;
    s_driver_mode     = DriverMode::ENABLED;
    s_vout_ref        = vout;
    s_iout_ref        = iout;
    return true;
  }


  void disablePowerConverter()
  {
    s_ltc7871_enabled = false;
    s_driver_mode     = DriverMode::DISABLED;
    s_vout_ref        = 0.0f;
  }


  void setVoutRef( const float voltage )
  {
    s_vout_ref = voltage;
  }


  void setIoutRef( const float current )
  {
    s_iout_ref = current;
  }


  uint32_t readFaults()
  {
    return s_ltc7871_faults;
  }


  DriverMode getMode()
  {
    return s_driver_mode;
  }


  void clearFaults()
  {
    s_ltc7871_faults = 0;
  }


  float getAverageOutputCurrent( float voltage )
  {
    ( void )voltage;
    return 0.0f;
  }


  void runFaultMonitoring()
  {
  }


  void runStateUpdater()
  {
  }
}    // namespace HW::LTC7871
