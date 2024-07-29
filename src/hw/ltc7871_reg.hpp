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
  static constexpr uint8_t REG_MIN_ADDR        = 0x01;
  static constexpr uint8_t REG_MAX_ADDR        = 0x0B;

  /*---------------------------------------------------------------------------
  MFR_FAULT
  ---------------------------------------------------------------------------*/
  static constexpr uint8_t MFR_FAULT_OVER_TEMP_Pos = 0;
  static constexpr uint8_t MFR_FAULT_OVER_TEMP_Msk = 1u << MFR_FAULT_OVER_TEMP_Pos;

  static constexpr uint8_t MFR_FAULT_VREF_BAD_Pos = 1;
  static constexpr uint8_t MFR_FAULT_VREF_BAD_Msk = 1u << MFR_FAULT_VREF_BAD_Pos;

  static constexpr uint8_t MFR_FAULT_V5_UV_Pos = 2;
  static constexpr uint8_t MFR_FAULT_V5_UV_Msk = 1u << MFR_FAULT_V5_UV_Pos;

  static constexpr uint8_t MFR_FAULT_DRVCC_UV_Pos = 3;
  static constexpr uint8_t MFR_FAULT_DRVCC_UV_Msk = 1u << MFR_FAULT_DRVCC_UV_Pos;

  static constexpr uint8_t MFR_FAULT_VHIGH_UV_Pos = 4;
  static constexpr uint8_t MFR_FAULT_VHIGH_UV_Msk = 1u << MFR_FAULT_VHIGH_UV_Pos;

  static constexpr uint8_t MFR_FAULT_VHIGH_OV_Pos = 5;
  static constexpr uint8_t MFR_FAULT_VHIGH_OV_Msk = 1u << MFR_FAULT_VHIGH_OV_Pos;

  static constexpr uint8_t MFR_FAULT_VLOW_OV_Pos = 6;
  static constexpr uint8_t MFR_FAULT_VLOW_OV_Msk = 1u << MFR_FAULT_VLOW_OV_Pos;

  /*---------------------------------------------------------------------------
  MFR_OC_FAULT
  ---------------------------------------------------------------------------*/
  static constexpr uint8_t MFR_OC_FAULT_CH1_Pos = 0;
  static constexpr uint8_t MFR_OC_FAULT_CH1_Msk = 1u << MFR_OC_FAULT_CH1_Pos;

  static constexpr uint8_t MFR_OC_FAULT_CH2_Pos = 1;
  static constexpr uint8_t MFR_OC_FAULT_CH2_Msk = 1u << MFR_OC_FAULT_CH2_Pos;

  static constexpr uint8_t MFR_OC_FAULT_CH3_Pos = 2;
  static constexpr uint8_t MFR_OC_FAULT_CH3_Msk = 1u << MFR_OC_FAULT_CH3_Pos;

  static constexpr uint8_t MFR_OC_FAULT_CH4_Pos = 3;
  static constexpr uint8_t MFR_OC_FAULT_CH4_Msk = 1u << MFR_OC_FAULT_CH4_Pos;

  static constexpr uint8_t MFR_OC_FAULT_CH5_Pos = 4;
  static constexpr uint8_t MFR_OC_FAULT_CH5_Msk = 1u << MFR_OC_FAULT_CH5_Pos;

  static constexpr uint8_t MFR_OC_FAULT_CH6_Pos = 5;
  static constexpr uint8_t MFR_OC_FAULT_CH6_Msk = 1u << MFR_OC_FAULT_CH6_Pos;

  /*---------------------------------------------------------------------------
  MFR_NOC_FAULT
  ---------------------------------------------------------------------------*/
  static constexpr uint8_t MFR_NOC_FAULT_CH1_Pos = 0;
  static constexpr uint8_t MFR_NOC_FAULT_CH1_Msk = 1u << MFR_NOC_FAULT_CH1_Pos;

  static constexpr uint8_t MFR_NOC_FAULT_CH2_Pos = 1;
  static constexpr uint8_t MFR_NOC_FAULT_CH2_Msk = 1u << MFR_NOC_FAULT_CH2_Pos;

  static constexpr uint8_t MFR_NOC_FAULT_CH3_Pos = 2;
  static constexpr uint8_t MFR_NOC_FAULT_CH3_Msk = 1u << MFR_NOC_FAULT_CH3_Pos;

  static constexpr uint8_t MFR_NOC_FAULT_CH4_Pos = 3;
  static constexpr uint8_t MFR_NOC_FAULT_CH4_Msk = 1u << MFR_NOC_FAULT_CH4_Pos;

  static constexpr uint8_t MFR_NOC_FAULT_CH5_Pos = 4;
  static constexpr uint8_t MFR_NOC_FAULT_CH5_Msk = 1u << MFR_NOC_FAULT_CH5_Pos;

  static constexpr uint8_t MFR_NOC_FAULT_CH6_Pos = 5;
  static constexpr uint8_t MFR_NOC_FAULT_CH6_Msk = 1u << MFR_NOC_FAULT_CH6_Pos;

  /*---------------------------------------------------------------------------
  MFR_STATUS
  ---------------------------------------------------------------------------*/
  static constexpr uint8_t MFR_STATUS_PGOOD_Pos = 0;
  static constexpr uint8_t MFR_STATUS_PGOOD_Msk = 1u << MFR_STATUS_PGOOD_Pos;

  static constexpr uint8_t MFR_STATUS_MAX_CURRENT_Pos = 1;
  static constexpr uint8_t MFR_STATUS_MAX_CURRENT_Msk = 1u << MFR_STATUS_MAX_CURRENT_Pos;

  static constexpr uint8_t MFR_STATUS_SS_DONE_Pos = 2;
  static constexpr uint8_t MFR_STATUS_SS_DONE_Msk = 1u << MFR_STATUS_SS_DONE_Pos;

  /*---------------------------------------------------------------------------
  MFR_CONFIG1
  ---------------------------------------------------------------------------*/
  static constexpr uint8_t MFR_CONFIG1_ILIM_SET_Pos  = 0;
  static constexpr uint8_t MFR_CONFIG1_ILIM_SET_Msk  = 0x07u << MFR_CONFIG1_ILIM_SET_Pos;
  static constexpr uint8_t MFR_CONFIG1_ILIM_SET_10mV = 0u << MFR_CONFIG1_ILIM_SET_Pos;
  static constexpr uint8_t MFR_CONFIG1_ILIM_SET_20mV = 1u << MFR_CONFIG1_ILIM_SET_Pos;
  static constexpr uint8_t MFR_CONFIG1_ILIM_SET_30mV = 2u << MFR_CONFIG1_ILIM_SET_Pos;
  static constexpr uint8_t MFR_CONFIG1_ILIM_SET_40mV = 3u << MFR_CONFIG1_ILIM_SET_Pos;
  static constexpr uint8_t MFR_CONFIG1_ILIM_SET_50mV = 4u << MFR_CONFIG1_ILIM_SET_Pos;

  static constexpr uint8_t MFR_CONFIG1_DRVCC_SET_Pos = 3;
  static constexpr uint8_t MFR_CONFIG1_DRVCC_SET_Msk = 0x03u << MFR_CONFIG1_DRVCC_SET_Pos;
  static constexpr uint8_t MFR_CONFIG1_DRVCC_SET_5V  = 0u << MFR_CONFIG1_DRVCC_SET_Pos;
  static constexpr uint8_t MFR_CONFIG1_DRVCC_SET_8V  = 1u << MFR_CONFIG1_DRVCC_SET_Pos;
  static constexpr uint8_t MFR_CONFIG1_DRVCC_SET_10V = 2u << MFR_CONFIG1_DRVCC_SET_Pos;

  static constexpr uint8_t MFR_CONFIG1_SERCUR_WARNING_Pos = 5;
  static constexpr uint8_t MFR_CONFIG1_SERCUR_WARNING_Msk = 1u << MFR_CONFIG1_SERCUR_WARNING_Pos;

  /*---------------------------------------------------------------------------
  MFR_CONFIG2
  ---------------------------------------------------------------------------*/
  static constexpr uint8_t MFR_CONFIG2_BUCK_BOOST_Pos   = 0;
  static constexpr uint8_t MFR_CONFIG2_BUCK_BOOST_Msk   = 1u << MFR_CONFIG2_BUCK_BOOST_Pos;
  static constexpr uint8_t MFR_CONFIG2_BUCK_BOOST_Boost = 0u << MFR_CONFIG2_BUCK_BOOST_Pos;
  static constexpr uint8_t MFR_CONFIG2_BUCK_BOOST_Buck  = 1u << MFR_CONFIG2_BUCK_BOOST_Pos;

  static constexpr uint8_t MFR_CONFIG2_SPRD_Pos = 1;
  static constexpr uint8_t MFR_CONFIG2_SPRD_Msk = 1u << MFR_CONFIG2_SPRD_Pos;

  static constexpr uint8_t MFR_CONFIG2_HIZ_Pos = 2;
  static constexpr uint8_t MFR_CONFIG2_HIZ_Msk = 1u << MFR_CONFIG2_HIZ_Pos;

  static constexpr uint8_t MFR_CONFIG2_DCM_Pos = 3;
  static constexpr uint8_t MFR_CONFIG2_DCM_Msk = 1u << MFR_CONFIG2_DCM_Pos;

  static constexpr uint8_t MFR_CONFIG2_BURST_Pos = 4;
  static constexpr uint8_t MFR_CONFIG2_BURST_Msk = 1u << MFR_CONFIG2_BURST_Pos;

  /*---------------------------------------------------------------------------
  MFR_CHIP_CTRL
  ---------------------------------------------------------------------------*/
  static constexpr uint8_t MFR_CHIP_CTRL_Msk = 0x07;

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

  /*---------------------------------------------------------------------------
  MFR_IDAC_VLOW / MFR_IDAC_VHIGH
  ---------------------------------------------------------------------------*/
  static constexpr uint8_t MFR_IDAC_V_Msk         = 0x7F;
  static constexpr uint8_t MFR_IDAC_V_SIGN_Pos    = 6;
  static constexpr uint8_t MFR_IDAC_V_SIGN_Msk    = 1u << MFR_IDAC_V_SIGN_Pos;
  static constexpr uint8_t MFR_IDAC_V_SIGN_SOURCE = 0u << MFR_IDAC_V_SIGN_Pos;
  static constexpr uint8_t MFR_IDAC_V_SIGN_SINK   = 1u << MFR_IDAC_V_SIGN_Pos;

  /*---------------------------------------------------------------------------
  MFR_IDAC_SETCUR
  ---------------------------------------------------------------------------*/
  static constexpr uint8_t MFR_IDAC_SETCUR_Msk = 0x1F;

  /*---------------------------------------------------------------------------
  MFR_SSFM
  ---------------------------------------------------------------------------*/
  static constexpr uint8_t MFR_SSFM_MOD_FREQ_Pos   = 0;
  static constexpr uint8_t MFR_SSFM_MOD_FREQ_Msk   = 0x07u << MFR_SSFM_MOD_FREQ_Pos;
  static constexpr uint8_t MFR_SSFM_MOD_FREQ_512   = 0u << MFR_SSFM_MOD_FREQ_Pos;
  static constexpr uint8_t MFR_SSFM_MOD_FREQ_1024  = 1u << MFR_SSFM_MOD_FREQ_Pos;
  static constexpr uint8_t MFR_SSFM_MOD_FREQ_2048  = 2u << MFR_SSFM_MOD_FREQ_Pos;
  static constexpr uint8_t MFR_SSFM_MOD_FREQ_4096  = 3u << MFR_SSFM_MOD_FREQ_Pos;
  static constexpr uint8_t MFR_SSFM_MOD_FREQ_256   = 4u << MFR_SSFM_MOD_FREQ_Pos;
  static constexpr uint8_t MFR_SSFM_MOD_FREQ_128   = 5u << MFR_SSFM_MOD_FREQ_Pos;
  static constexpr uint8_t MFR_SSFM_MOD_FREQ_64    = 6u << MFR_SSFM_MOD_FREQ_Pos;
  static constexpr uint8_t MFR_SSFM_MOD_FREQ_512_2 = 7u << MFR_SSFM_MOD_FREQ_Pos;

  static constexpr uint8_t MFR_SSFM_FREQ_SPREAD_Pos = 3;
  static constexpr uint8_t MFR_SSFM_FREQ_SPREAD_Msk = 0x03u << MFR_SSFM_FREQ_SPREAD_Pos;
  static constexpr uint8_t MFR_SSFM_FREQ_SPREAD_12  = 0u << MFR_SSFM_FREQ_SPREAD_Pos;
  static constexpr uint8_t MFR_SSFM_FREQ_SPREAD_15  = 1u << MFR_SSFM_FREQ_SPREAD_Pos;
  static constexpr uint8_t MFR_SSFM_FREQ_SPREAD_10  = 2u << MFR_SSFM_FREQ_SPREAD_Pos;
  static constexpr uint8_t MFR_SSFM_FREQ_SPREAD_8   = 3u << MFR_SSFM_FREQ_SPREAD_Pos;
}    // namespace HW::LTC7871

#endif /* !ICHNAEA_LTC7871_REGISTERS_HPP */
