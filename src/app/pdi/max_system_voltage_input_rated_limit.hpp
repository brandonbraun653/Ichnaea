#pragma once
#ifndef ICHNAEA_APP_PDI_MAX_SYSTEM_VOLTAGE_INPUT_RATED_LIMIT_HPP
#define ICHNAEA_APP_PDI_MAX_SYSTEM_VOLTAGE_INPUT_RATED_LIMIT_HPP

#include <src/app/app_pdi.hpp>
#include <src/app/proto/ichnaea_pdi.pb.h>

namespace App::PDI
{
  bool setMaxSystemVoltageInputRatedLimit( float value );
  float getMaxSystemVoltageInputRatedLimit();
  void pdi_register_key__max_system_voltage_input_rated_limit();
}

#endif /* !ICHNAEA_APP_PDI_MAX_SYSTEM_VOLTAGE_INPUT_RATED_LIMIT_HPP */
