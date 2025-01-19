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
#include <algorithm>
#include <src/hw/fan.hpp>

namespace HW::FAN
{
  /*-------------------------------------------------------------------------
  Private Data
  -------------------------------------------------------------------------*/

  static float s_fan_speed_rpm;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    s_fan_speed_rpm = 10.0f;  // Initialize to default value
  }


  void postSequence()
  {
  }


  void setSpeedRPM( const float speed )
  {
    s_fan_speed_rpm = std::clamp( speed, 0.0f, 6000.0f );
  }


  float getFanSpeed()
  {
    return s_fan_speed_rpm;
  }

}    // namespace HW::FAN
