#pragma once
#ifndef ICHNAEA_APP_PDI_CONFIG_MIN_SYSTEM_VOLTAGE_INPUT_HPP
#define ICHNAEA_APP_PDI_CONFIG_MIN_SYSTEM_VOLTAGE_INPUT_HPP

#include <src/app/app_pdi.hpp>

namespace App::PDI
{
  bool setConfigMinSystemVoltageInput(float value);
  float getConfigMinSystemVoltageInput();
  void pdi_register_key__config_min_system_voltage_input();
}

#endif /* !ICHNAEA_APP_PDI_CONFIG_MIN_SYSTEM_VOLTAGE_INPUT_HPP */
