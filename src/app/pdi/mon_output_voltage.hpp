#pragma once
#ifndef ICHNAEA_APP_PDI_MON_OUTPUT_VOLTAGE_HPP
#define ICHNAEA_APP_PDI_MON_OUTPUT_VOLTAGE_HPP

#include <src/app/app_pdi.hpp>

#include <src/app/proto/ichnaea_pdi.pb.h>

namespace App::PDI
{
  bool setMonOutputVoltageRaw(float value);
  float getMonOutputVoltageRaw();

  bool setMonOutputVoltageFiltered(float value);
  float getMonOutputVoltageFiltered();

  bool setMonOutputVoltageValid(bool value);
  bool getMonOutputVoltageValid();

  bool setMonLoadVoltagePctErrorOORLimit(float value);
  float getMonLoadVoltagePctErrorOORLimit();

  bool setMonLoadVoltagePctErrorOOREntryDelayMS(uint32_t value);
  uint32_t getMonLoadVoltagePctErrorOOREntryDelayMS();

  bool setMonLoadVoltagePctErrorOORExitDelayMS(uint32_t value);
  uint32_t getMonLoadVoltagePctErrorOORExitDelayMS();

  bool setMonFilterOutputVoltage( ichnaea_PDI_IIRFilterConfig &config );
  ichnaea_PDI_IIRFilterConfig getMonFilterOutputVoltage();

  void pdi_register_key__output_voltage_raw();
  void pdi_register_key__output_voltage_filter();
  void pdi_register_key__output_voltage_filtered();
  void pdi_register_key__output_voltage_valid();
  void pdi_register_key__config_mon_load_voltage_pct_error_oor_limit();
  void pdi_register_key__config_mon_load_voltage_pct_error_oor_entry_delay_ms();
  void pdi_register_key__config_mon_load_voltage_pct_error_oor_exit_delay_ms();
}

#endif /* !ICHNAEA_APP_PDI_MON_OUTPUT_VOLTAGE_HPP */
