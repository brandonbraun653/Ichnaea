/******************************************************************************
 *  File Name:
 *    app_config.cpp
 *
 *  Description:
 *    Ichnaea system configuration settings
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/assert.hpp>
#include <src/app/app_config.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/proto/ichnaea_pdi.pb.h>
#include <src/system/system_db.hpp>
#include <src/app/pdi/target_fan_speed_rpm.hpp>
#include <src/app/pdi/config_min_temp_limit.hpp>
#include <src/app/pdi/config_max_temp_limit.hpp>

namespace App::Config
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void driver_init()
  {
    App::PDI::pdi_register_key__target_fan_speed_rpm();
    App::PDI::pdi_register_key__config_min_temp_limit();
    App::PDI::pdi_register_key__config_max_temp_limit();
  }


  void driver_deinit()
  {
  }

}  // namespace App::Config
