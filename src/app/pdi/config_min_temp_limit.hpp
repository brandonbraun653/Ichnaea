#pragma once
#ifndef ICHNAEA_APP_PDI_CONFIG_MIN_TEMP_LIMIT_HPP
#define ICHNAEA_APP_PDI_CONFIG_MIN_TEMP_LIMIT_HPP

#include <src/app/app_pdi.hpp>

namespace App::PDI
{
  bool setConfigMinTempLimit(float value);
  float getConfigMinTempLimit();
  void pdi_register_key__config_min_temp_limit();
}

#endif /* !ICHNAEA_APP_PDI_CONFIG_MIN_TEMP_LIMIT_HPP */
