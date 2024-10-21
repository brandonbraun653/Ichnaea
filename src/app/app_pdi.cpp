/******************************************************************************
 *  File Name:
 *    app_pdi.cpp
 *
 *  Description:
 *    Application Programmable Data Item (PDI) interface implementation
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <src/app/app_pdi.hpp>

namespace App::PDI
{
  /*---------------------------------------------------------------------------
  Public Data
  ---------------------------------------------------------------------------*/

  PDIData Internal::RAMCache; /**< RAM cache for the PDI database */

}  // namespace App::PDI
