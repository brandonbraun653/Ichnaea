#pragma once
#ifndef ICHNAEA_APP_PDI_CONFIG_MAX_TEMP_LIMIT_HPP
#define ICHNAEA_APP_PDI_CONFIG_MAX_TEMP_LIMIT_HPP

#include <src/app/app_pdi.hpp>

namespace App::PDI
{
  bool setConfigMaxTempLimit(float value);
  float getConfigMaxTempLimit();
  void pdi_register_key__config_max_temp_limit();
}

#endif /* !ICHNAEA_APP_PDI_CONFIG_MAX_TEMP_LIMIT_HPP */
