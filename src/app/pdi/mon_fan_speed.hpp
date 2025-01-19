#pragma once
#ifndef ICHNAEA_APP_PDI_MON_FAN_SPEED_HPP
#define ICHNAEA_APP_PDI_MON_FAN_SPEED_HPP

#include <src/app/app_pdi.hpp>

#include <src/app/proto/ichnaea_pdi.pb.h>

namespace App::PDI
{
  bool setMonFanSpeedFiltered(float value);
  float getMonFanSpeedFiltered();

  bool setMonFanSpeedValid(bool value);
  bool getMonFanSpeedValid();

  bool setMonFanSpeedPctErrorOORLimit(float value);
  float getMonFanSpeedPctErrorOORLimit();

  bool setMonFanSpeedOOREntryDelayMS(uint32_t value);
  uint32_t getMonFanSpeedOOREntryDelayMS();

  bool setMonFanSpeedOORExitDelayMS(uint32_t value);
  uint32_t getMonFanSpeedOORExitDelayMS();

  bool setMonFilterFanSpeed( ichnaea_PDI_IIRFilterConfig &config );
  ichnaea_PDI_IIRFilterConfig getMonFilterFanSpeed();

  void pdi_register_key__fan_speed_filter();
  void pdi_register_key__fan_speed_filtered();
  void pdi_register_key__fan_speed_valid();
  void pdi_register_key__config_mon_fan_speed_pct_error_oor_limit();
  void pdi_register_key__config_mon_fan_speed_oor_entry_delay_ms();
  void pdi_register_key__config_mon_fan_speed_oor_exit_delay_ms();
}

#endif /* !ICHNAEA_APP_PDI_MON_FAN_SPEED_HPP */
