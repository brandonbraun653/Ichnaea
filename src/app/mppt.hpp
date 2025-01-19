/******************************************************************************
 *  File Name:
 *    mppt.hpp
 *
 *  Description:
 *    Maximum Power Point Tracking (MPPT) algorithm implementation
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_MPPT_HPP
#define ICHNAEA_MPPT_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>

namespace App::Mppt
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize();

  /**
   * @brief Sets the maximum current the output can provide
   *
   * @param limit Current limit in Amps
   */
  void setOutputCurrentLimit( const float limit );

  /**
   * @brief Sets the target output voltage of Ichnaea
   *
   * This does not immediately reflect on the output if the power supply is not
   * enabled. That control is performed elsewhere.
   *
   * @param voltage Reference to set in volts
   */
  void setOutputVoltageReference( const float voltage );

}  // namespace APP::Mppt

#endif  /* !ICHNAEA_MPPT_HPP */
