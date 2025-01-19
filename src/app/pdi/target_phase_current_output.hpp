#pragma once
#ifndef ICHNAEA_APP_PDI_TARGET_PHASE_CURRENT_OUTPUT_HPP
#define ICHNAEA_APP_PDI_TARGET_PHASE_CURRENT_OUTPUT_HPP

namespace App::PDI
{
  bool setTargetPhaseCurrentOutput( float value );
  float getTargetPhaseCurrentOutput();
  void pdi_register_key__target_phase_current_output();
}

#endif /* !ICHNAEA_APP_PDI_TARGET_PHASE_CURRENT_OUTPUT_HPP */
