#pragma once
#ifndef ICHNAEA_APP_PDI_MON_INPUT_VOLTAGE_HPP
#define ICHNAEA_APP_PDI_MON_INPUT_VOLTAGE_HPP

#include <src/app/app_pdi.hpp>

#include <src/app/proto/ichnaea_pdi.pb.h>

namespace App::PDI
{
  bool setMonInputVoltageRaw(float value);
  float getMonInputVoltageRaw();

  bool setMonInputVoltageFiltered(float value);
  float getMonInputVoltageFiltered();

  bool setMonInputVoltageValid(bool value);
  bool getMonInputVoltageValid();

  bool setMonInputVoltageOOREntryDelayMS(uint32_t value);
  uint32_t getMonInputVoltageOOREntryDelayMS();

  bool setMonInputVoltageOORExitDelayMS(uint32_t value);
  uint32_t getMonInputVoltageOORExitDelayMS();

  bool setMonFilterInputVoltage( ichnaea_PDI_IIRFilterConfig &config );
  ichnaea_PDI_IIRFilterConfig getMonFilterInputVoltage();

  void pdi_register_key__input_voltage_raw();
  void pdi_register_key__input_voltage_filter();
  void pdi_register_key__input_voltage_filtered();
  void pdi_register_key__input_voltage_valid();
  void pdi_register_key__config_mon_input_voltage_oor_entry_delay_ms();
  void pdi_register_key__config_mon_input_voltage_oor_exit_delay_ms();
}

#endif /* !ICHNAEA_APP_PDI_MON_INPUT_VOLTAGE_HPP */
