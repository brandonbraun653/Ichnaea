/******************************************************************************
 *  File Name:
 *    bms_client.cpp
 *
 *  Description:
 *    Ichnaea driver for talking with the remote BMS
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <src/com/bms_client.hpp>

namespace COM::BMS
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  bool isAvailable()
  {
    // Just a stub for compilation purposes. This will be expanded later.
    // I needed a concrete function to call for the LTC power up sequence
    // as it tries to resolve the boot configuration.

    // TODO BMB: Ping the remote node each time to ensure we have liveness.
    return false;
  }

  bool getRequirements( BMSRequirements &req )
  {
    // Just a stub for compilation purposes. This will be expanded later.
    // I needed a concrete function to call for the LTC power up sequence
    // as it tries to resolve the boot configuration.
    return false;
  }

}  // namespace COM::BMS
