/******************************************************************************
 *  File Name:
 *    bms_client.hpp
 *
 *  Description:
 *    Ichnaea driver for talking with the remote BMS
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_BMS_CLIENT_HPP
#define ICHNAEA_BMS_CLIENT_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/


namespace COM::BMS
{
  /*---------------------------------------------------------------------------
  Structures
  ---------------------------------------------------------------------------*/

  /**
   * @brief Core interface to the BMS that provides power targets
   *
   * TODO BMB: This should get swapped for protocol buffers once mature.
   * Need to standardize on a common interface for all external systems.
   */
  struct BMSRequirements
  {
    float voltageLimit;
    float voltageProgram;
    float currentLimit;
  };

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize();

  void postSequence();

  bool isAvailable();

  bool getRequirements( BMSRequirements &req );

}  // namespace

#endif  /* !ICHNAEA_BMS_CLIENT_HPP */
