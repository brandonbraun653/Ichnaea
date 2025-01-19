#pragma once
#ifndef ICHNAEA_APP_PDI_MON_OUTPUT_CURRENT_HPP
#define ICHNAEA_APP_PDI_MON_OUTPUT_CURRENT_HPP

#include <src/app/app_pdi.hpp>

#include <src/app/proto/ichnaea_pdi.pb.h>

namespace App::PDI
{
  bool setMonOutputCurrentRaw(float value);
  float getMonOutputCurrentRaw();

  bool setMonOutputCurrentFiltered(float value);
  float getMonOutputCurrentFiltered();

  bool setMonOutputCurrentValid(bool value);
  bool getMonOutputCurrentValid();

  bool setMonLoadOvercurrentOOREntryDelayMS(uint32_t value);
  uint32_t getMonLoadOvercurrentOOREntryDelayMS();

  bool setMonLoadOvercurrentOORExitDelayMS(uint32_t value);
  uint32_t getMonLoadOvercurrentOORExitDelayMS();

  bool setMonFilterOutputCurrent( ichnaea_PDI_IIRFilterConfig &config );
  ichnaea_PDI_IIRFilterConfig getMonFilterOutputCurrent();

  void pdi_register_key__output_current_raw();
  void pdi_register_key__output_current_filter();
  void pdi_register_key__output_current_filtered();
  void pdi_register_key__output_current_valid();
  void pdi_register_key__config_mon_load_overcurrent_oor_entry_delay_ms();
  void pdi_register_key__config_mon_load_overcurrent_oor_exit_delay_ms();
}

#endif /* !ICHNAEA_APP_PDI_MON_OUTPUT_CURRENT_HPP */
