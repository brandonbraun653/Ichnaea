#pragma once
#ifndef ICHNAEA_APP_PDI_MON_1V1_VOLTAGE_HPP
#define ICHNAEA_APP_PDI_MON_1V1_VOLTAGE_HPP

#include <src/app/app_pdi.hpp>

#include <src/app/proto/ichnaea_pdi.pb.h>

namespace App::PDI
{
  bool setMon1V1VoltageFiltered(float value);
  float getMon1V1VoltageFiltered();

  bool setMon1V1VoltageValid(bool value);
  bool getMon1V1VoltageValid();

  bool setMonFilter1V1Voltage( ichnaea_PDI_IIRFilterConfig &config );
  ichnaea_PDI_IIRFilterConfig getMonFilter1V1Voltage();

  void pdi_register_key__1v1_voltage_filter();
  void pdi_register_key__1v1_voltage_filtered();
  void pdi_register_key__1v1_voltage_valid();
}

#endif /* !ICHNAEA_APP_PDI_MON_1V1_VOLTAGE_HPP */
