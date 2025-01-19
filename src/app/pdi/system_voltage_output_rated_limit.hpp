#pragma once
#ifndef ICHNAEA_APP_PDI_SYSTEM_VOLTAGE_OUTPUT_RATED_LIMIT_HPP
#define ICHNAEA_APP_PDI_SYSTEM_VOLTAGE_OUTPUT_RATED_LIMIT_HPP

#include <src/app/app_pdi.hpp>
#include <src/app/proto/ichnaea_pdi.pb.h>

namespace App::PDI
{
  bool setSystemVoltageOutputRatedLimit( float value );
  float getSystemVoltageOutputRatedLimit();
  void pdi_register_key__system_voltage_output_rated_limit();
}

#endif /* !ICHNAEA_APP_PDI_SYSTEM_VOLTAGE_OUTPUT_RATED_LIMIT_HPP */
