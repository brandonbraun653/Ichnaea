#pragma once
#ifndef ICHNAEA_APP_PDI_LTC7871_HPP
#define ICHNAEA_APP_PDI_LTC7871_HPP

#include <src/app/app_pdi.hpp>
#include <src/app/proto/ichnaea_pdi.pb.h>

namespace App::PDI
{
  bool setLTCPhaseInductorDCR( float value );
  float getLTCPhaseInductorDCR();
  void pdi_register_key__config_ltc_phase_inductor_dcr();
}

#endif /* !ICHNAEA_APP_PDI_LTC7871_HPP */
