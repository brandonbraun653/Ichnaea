#pragma once
#ifndef ICHNAEA_APP_PDI_PGOOD_MONITOR_TIMEOUT_MS_HPP
#define ICHNAEA_APP_PDI_PGOOD_MONITOR_TIMEOUT_MS_HPP

#include <src/app/app_pdi.hpp>
#include <src/app/proto/ichnaea_pdi.pb.h>

namespace App::PDI
{
  bool setPgoodMonitorTimeoutMS( uint32_t value );
  uint32_t getPgoodMonitorTimeoutMS();
  void pdi_register_key__pgood_monitor_timeout_ms();
}

#endif /* !ICHNAEA_APP_PDI_PGOOD_MONITOR_TIMEOUT_MS_HPP */
