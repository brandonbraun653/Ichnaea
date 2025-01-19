#pragma once
#ifndef ICHNAEA_APP_PDI_CONFIG_MAX_SYSTEM_VOLTAGE_INPUT_HPP
#define ICHNAEA_APP_PDI_CONFIG_MAX_SYSTEM_VOLTAGE_INPUT_HPP

#include <src/app/app_pdi.hpp>

namespace App::PDI
{
  bool setConfigMaxSystemVoltageInput(float value);
  float getConfigMaxSystemVoltageInput();
  void pdi_register_key__config_max_system_voltage_input();
}

#endif /* !ICHNAEA_APP_PDI_CONFIG_MAX_SYSTEM_VOLTAGE_INPUT_HPP */
