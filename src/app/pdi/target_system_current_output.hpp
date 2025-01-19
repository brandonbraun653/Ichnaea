
#pragma once
#ifndef ICHNAEA_APP_PDI_TARGET_SYSTEM_CURRENT_OUTPUT_HPP
#define ICHNAEA_APP_PDI_TARGET_SYSTEM_CURRENT_OUTPUT_HPP

#include <src/app/app_pdi.hpp>

#include <src/app/proto/ichnaea_pdi.pb.h>

namespace App::PDI
{
  bool setTargetSystemCurrentOutput( float value );
  float getTargetSystemCurrentOutput();
  void pdi_register_key__target_system_current_output();
}

#endif /* !ICHNAEA_APP_PDI_TARGET_SYSTEM_CURRENT_OUTPUT_HPP */