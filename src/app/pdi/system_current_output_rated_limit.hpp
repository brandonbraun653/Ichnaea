#pragma once
#ifndef ICHNAEA_APP_PDI_SYSTEM_CURRENT_OUTPUT_RATED_LIMIT_HPP
#define ICHNAEA_APP_PDI_SYSTEM_CURRENT_OUTPUT_RATED_LIMIT_HPP

namespace App::PDI
{
  bool setSystemCurrentOutputRatedLimit( float value );
  float getSystemCurrentOutputRatedLimit();
  void pdi_register_key__system_current_output_rated_limit();
}

#endif /* !ICHNAEA_APP_PDI_SYSTEM_CURRENT_OUTPUT_RATED_LIMIT_HPP */
