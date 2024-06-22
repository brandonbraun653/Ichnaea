/******************************************************************************
 *  File Name:
 *    ltc7871_reg.hpp
 *
 *  Description:
 *    Register definitions for the LTC7871
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_LTC7871_REGISTERS_HPP
#define ICHNAEA_LTC7871_REGISTERS_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>

namespace HW::LTC7871
{
  /*---------------------------------------------------------------------------
  Register Definitions
  ---------------------------------------------------------------------------*/
  static constexpr uint8_t REG_MFR_FAULT       = 0x01; /**< (RO) Summary of the unit’s fault condition. */
  static constexpr uint8_t REG_MFR_OC_FAULT    = 0x02; /**< (RO) Summary of the unit's overcurrent fault condition */
  static constexpr uint8_t REG_MFR_NOC_FAULT   = 0x03; /**< (RO) Summary of the unit's negative overcurrent fault condition */
  static constexpr uint8_t REG_MFR_STATUS      = 0x04; /**< (RO) Summary of the unit's operation status */
  static constexpr uint8_t REG_MFR_CONFIG1     = 0x05; /**< (RO) Summary of the unit's configuration */
  static constexpr uint8_t REG_MFR_CONFIG2     = 0x06; /**< (RO) Summary of the unit's configuration */
  static constexpr uint8_t REG_MFR_CHIP_CTRL   = 0x07; /**< (RW) General chip control */
  static constexpr uint8_t REG_MFR_IDAC_VLOW   = 0x08; /**< (RW) Program VLOW voltage */
  static constexpr uint8_t REG_MFR_IDAC_VHIGH  = 0x09; /**< (RW) Program VHIGH voltage */
  static constexpr uint8_t REG_MFR_IDAC_SETCUR = 0x0A; /**< (RW) Program the SETCUR pin's sourcing current */
  static constexpr uint8_t REG_MFR_SSFM        = 0x0B; /**< (RW) Adjust the spread spectrum frequency modulation parameters */
  static constexpr uint8_t REG_MAX_ADDR        = 0x0B;

  /*---------------------------------------------------------------------------
  MFR_CHIP_CTRL
  ---------------------------------------------------------------------------*/
  static constexpr uint8_t MFR_CHIP_CTRL_WP_Pos     = 0;
  static constexpr uint8_t MFR_CHIP_CTRL_WP_Msk     = 1u << MFR_CHIP_CTRL_WP_Pos;
  static constexpr uint8_t MFR_CHIP_CTRL_WP_Disable = 0u << MFR_CHIP_CTRL_WP_Pos;
  static constexpr uint8_t MFR_CHIP_CTRL_WP_Enable  = 1u << MFR_CHIP_CTRL_WP_Pos;

  static constexpr uint8_t MFR_CHIP_CTRL_RESET_Pos = 1;
  static constexpr uint8_t MFR_CHIP_CTRL_RESET_Msk = 1u << MFR_CHIP_CTRL_RESET_Pos;
  static constexpr uint8_t MFR_CHIP_CTRL_RESET     = MFR_CHIP_CTRL_RESET_Msk;

  static constexpr uint8_t MFR_CHIP_CTRL_CML_Pos    = 2;
  static constexpr uint8_t MFR_CHIP_CTRL_CML_Msk    = 1u << MFR_CHIP_CTRL_CML_Pos;
  static constexpr uint8_t MFR_CHIP_CTRL_CML_Normal = 0u << MFR_CHIP_CTRL_CML_Pos;
  static constexpr uint8_t MFR_CHIP_CTRL_CML_Fault  = 1u << MFR_CHIP_CTRL_CML_Pos;

}  // namespace HW::LTC7871

#endif  /* !ICHNAEA_LTC7871_REGISTERS_HPP */
