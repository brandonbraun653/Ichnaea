#pragma once
#ifndef ICHNAEA_APP_PDI_MON_5V0_VOLTAGE_HPP
#define ICHNAEA_APP_PDI_MON_5V0_VOLTAGE_HPP

#include <src/app/app_pdi.hpp>

#include <src/app/proto/ichnaea_pdi.pb.h>

namespace App::PDI
{
  bool setMon5V0VoltageFiltered(float value);
  float getMon5V0VoltageFiltered();

  bool setMon5V0VoltageValid(bool value);
  bool getMon5V0VoltageValid();

  bool setMonFilter5V0Voltage( ichnaea_PDI_IIRFilterConfig &config );
  ichnaea_PDI_IIRFilterConfig getMonFilter5V0Voltage();

  void pdi_register_key__5v0_voltage_filter();
  void pdi_register_key__5v0_voltage_filtered();
  void pdi_register_key__5v0_voltage_valid();
}

#endif /* !ICHNAEA_APP_PDI_MON_5V0_VOLTAGE_HPP */
