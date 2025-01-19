#pragma once
#ifndef ICHNAEA_APP_PDI_PHASE_CURRENT_OUTPUT_RATED_LIMIT_HPP
#define ICHNAEA_APP_PDI_PHASE_CURRENT_OUTPUT_RATED_LIMIT_HPP

namespace App::PDI
{
  bool setPhaseCurrentOutputRatedLimit( float value );
  float getPhaseCurrentOutputRatedLimit();
  void pdi_register_key__phase_current_output_rated_limit();
}

#endif /* !ICHNAEA_APP_PDI_PHASE_CURRENT_OUTPUT_RATED_LIMIT_HPP */
