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
#include "etl/array.h"
#include "src/bsp/board_map.hpp"
#include "src/hw/adc.hpp"

namespace HW::ADC
{
  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static etl::array<float, Channel::NUM_OPTIONS> s_cached_voltage;
  static recursive_mutex_t                       s_adc_mutex;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {

  }


  void postInit()
  {

  }


  float getVoltage( const Channel channel )
  {
    return 0.0f;
  }


  float getCachedVoltage( const Channel channel )
  {
    return 0.0f;
  }
}  // namespace HW::ADC
