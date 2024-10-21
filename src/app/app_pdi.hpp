/******************************************************************************
 *  File Name:
 *    app_pdi.hpp
 *
 *  Description:
 *    Application Programmable Data Item (PDI) interface. This enumerates the
 *    configurable system data that can affect runtime behavior or record data
 *    from the system.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_APP_PDI_HPP
#define ICHNAEA_APP_PDI_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>
#include <mbedutils/database.hpp>
#include <src/app/proto/ichnaea_pdi.pb.h>

namespace App::PDI
{
  /*---------------------------------------------------------------------------
  Enumerations
  ---------------------------------------------------------------------------*/

  /**
   * @brief Keys for accessing data stored in the PDI database
   * @warning Do not change the values of these keys once they are in use.
   */
  enum PDIKey : mb::db::HashKey
  {
    /*-------------------------------------------------------------------------
    System Descriptors
    -------------------------------------------------------------------------*/
    KEY_BOOT_COUNT    = 0,  /**< Number of times the system has booted */
    KEY_SERIAL_NUMBER = 1,  /**< Unique serial number of the system */
    KEY_MFG_DATE      = 2,  /**< Date the system was manufactured */
    KEY_CAL_DATE      = 3,  /**< Date the system was last calibrated */

  };

  /*---------------------------------------------------------------------------
  Structures
  ---------------------------------------------------------------------------*/

  /**
   * @brief A RAM cache backing for the PDI database.
   *
   * Order does not matter here, but there is a 1:1 relationship between
   * the keys, the data items, and the database entry descriptors.
   */
  struct PDIData
  {
    uint32_t bootCount;    /**< KEY_BOOT_COUNT */
  };

  /*---------------------------------------------------------------------------
  Public Data
  ---------------------------------------------------------------------------*/
  namespace Internal
  {
    extern PDIData RAMCache;  /**< RAM cache for the PDI database */
  }

}  // namespace App::PDI

#endif  /* !ICHNAEA_APP_PDI_HPP */
