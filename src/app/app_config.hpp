/******************************************************************************
 *  File Name:
 *    app_config.hpp
 *
 *  Description:
 *    Ichnaea system configuration settings
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_APP_CONFIG_HPP
#define ICHNAEA_APP_CONFIG_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/


namespace App::Config
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void driver_init();

  void driver_deinit();

  // TODO: Maybe do some on-target calibrations:
  // - Fan speed range detection
  // - Sensor noise floor detection

}  // namespace App::Config

#endif  /* !ICHNAEA_APP_CONFIG_HPP */
