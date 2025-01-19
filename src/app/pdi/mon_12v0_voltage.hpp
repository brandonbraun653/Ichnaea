#pragma once
#ifndef ICHNAEA_APP_PDI_MON_12V0_VOLTAGE_HPP
#define ICHNAEA_APP_PDI_MON_12V0_VOLTAGE_HPP

#include <src/app/app_pdi.hpp>

#include <src/app/proto/ichnaea_pdi.pb.h>

namespace App::PDI
{
  bool setMon12V0VoltageFiltered(float value);
  float getMon12V0VoltageFiltered();

  bool setMon12V0VoltageValid(bool value);
  bool getMon12V0VoltageValid();

  bool setMonFilter12V0Voltage( ichnaea_PDI_IIRFilterConfig &config );
  ichnaea_PDI_IIRFilterConfig getMonFilter12V0Voltage();

  void pdi_register_key__12v0_voltage_filter();
  void pdi_register_key__12v0_voltage_filtered();
  void pdi_register_key__12v0_voltage_valid();
}

#endif /* !ICHNAEA_APP_PDI_MON_12V0_VOLTAGE_HPP */
