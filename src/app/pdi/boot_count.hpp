#pragma once
#ifndef ICHNAEA_APP_PDI_BOOT_COUNT_HPP
#define ICHNAEA_APP_PDI_BOOT_COUNT_HPP

#include <src/app/app_pdi.hpp>
#include <src/app/proto/ichnaea_pdi.pb.h>

namespace App::PDI
{
  bool setBootCount( uint32_t value );
  uint32_t getBootCount();
  void pdi_register_key_boot_count();
}

#endif /* !ICHNAEA_APP_PDI_BOOT_COUNT_HPP */
