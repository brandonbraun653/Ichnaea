/******************************************************************************
 *  File Name:
 *    bootup.cpp
 *
 *  Description:
 *    Bootup routines for the hardware on startup
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "src/bsp/board_map.hpp"
#include "src/hw/bootup.hpp"
#include "src/system/system_error.hpp"

namespace HW
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/
  void initDrivers()
  {
    Panic::powerUp();
    BSP::powerUp();
    //HW::LED::initialize();
  }

  void runPostInit()
  {
    //HW::LED::set( HW::LED::Channel::STATUS_0, true );
    //HW::LED::set( HW::LED::Channel::STATUS_1, true );
    //HW::LED::set( HW::LED::Channel::STATUS_2, true );
    //HW::LED::set( HW::LED::Channel::STATUS_3, true );
  }

}  // namespace HW
