#pragma once
#ifndef ICHNAEA_APP_PDI_CAL_OUTPUT_CURRENT_HPP
#define ICHNAEA_APP_PDI_CAL_OUTPUT_CURRENT_HPP

#include <src/app/app_pdi.hpp>
#include <src/app/proto/ichnaea_pdi.pb.h>

namespace App::PDI
{
  bool setCalOutputCurrent( ichnaea_PDI_BasicCalibration &value );
  void getCalOutputCurrent( ichnaea_PDI_BasicCalibration &value );
  void pdi_register_key_cal_output_current();
}    // namespace App::PDI

#endif /* !ICHNAEA_APP_PDI_CAL_OUTPUT_CURRENT_HPP */
