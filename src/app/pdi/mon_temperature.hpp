#pragma once
#ifndef ICHNAEA_APP_PDI_MON_TEMPERATURE_HPP
#define ICHNAEA_APP_PDI_MON_TEMPERATURE_HPP

#include <src/app/app_pdi.hpp>

#include <src/app/proto/ichnaea_pdi.pb.h>

namespace App::PDI
{
  bool setMonTemperatureFiltered(float value);
  float getMonTemperatureFiltered();

  bool setMonTemperatureValid(bool value);
  bool getMonTemperatureValid();

  bool setMonTemperatureOOREntryDelayMS(uint32_t value);
  uint32_t getMonTemperatureOOREntryDelayMS();

  bool setMonTemperatureOORExitDelayMS(uint32_t value);
  uint32_t getMonTemperatureOORExitDelayMS();

  bool setMonFilterTemperature( ichnaea_PDI_IIRFilterConfig &config );
  ichnaea_PDI_IIRFilterConfig getMonFilterTemperature();

  void pdi_register_key__temperature_filter();
  void pdi_register_key__temperature_filtered();
  void pdi_register_key__temperature_valid();
  void pdi_register_key__config_mon_temperature_oor_entry_delay_ms();
  void pdi_register_key__config_mon_temperature_oor_exit_delay_ms();
}

#endif /* !ICHNAEA_APP_PDI_MON_TEMPERATURE_HPP */
