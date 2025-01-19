#pragma once
#ifndef ICHNAEA_APP_PDI_TARGET_FAN_SPEED_RPM_HPP
#define ICHNAEA_APP_PDI_TARGET_FAN_SPEED_RPM_HPP

#include <src/app/app_pdi.hpp>

namespace App::PDI
{
  bool setTargetFanSpeedRPM(float value);
  float getTargetFanSpeedRPM();
  void pdi_register_key__target_fan_speed_rpm();
}

#endif /* !ICHNAEA_APP_PDI_TARGET_FAN_SPEED_RPM_HPP */
