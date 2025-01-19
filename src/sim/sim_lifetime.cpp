/******************************************************************************
 *  File Name:
 *    sim_lifetime.cpp
 *
 *  Description:
 *    Lifecycle management for the simulation environment
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <src/sim/sim_services.hpp>
#include <src/sim/sim_load.hpp>

namespace SIM
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    SIM::spawnServices();
    SIM::Load::initialize();
  }


  void shutdown()
  {
    SIM::destroyServices();
  }
}  // namespace SIM
