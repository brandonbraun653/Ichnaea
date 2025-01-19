#pragma once
#ifndef ICHNAEA_APP_PDI_MON_3V3_VOLTAGE_HPP
#define ICHNAEA_APP_PDI_MON_3V3_VOLTAGE_HPP

#include <src/app/app_pdi.hpp>

#include <src/app/proto/ichnaea_pdi.pb.h>

namespace App::PDI
{
  bool setMon3V3VoltageFiltered(float value);
  float getMon3V3VoltageFiltered();

  bool setMon3V3VoltageValid(bool value);
  bool getMon3V3VoltageValid();

  bool setMonFilter3V3Voltage( ichnaea_PDI_IIRFilterConfig &config );
  ichnaea_PDI_IIRFilterConfig getMonFilter3V3Voltage();

  void pdi_register_key__3v3_voltage_filter();
  void pdi_register_key__3v3_voltage_filtered();
  void pdi_register_key__3v3_voltage_valid();
}

#endif /* !ICHNAEA_APP_PDI_MON_3V3_VOLTAGE_HPP */
