/******************************************************************************
 *  File Name:
 *    fal_cfg.h
 *
 *  Description:
 *    FlashDB "Flash Abstraction Library" Configuration header for Ichanea
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#ifndef ICHNAEA_FAL_CFG_H
#define ICHNAEA_FAL_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <fdb_cfg.h>

/*-----------------------------------------------------------------------------
Literals
-----------------------------------------------------------------------------*/

#define FAL_PRINTF
#define ICHNAEA_DB_FLASH_DEV_NAME "nor_flash_0"
#define ICHNAEA_DB_GAIN_RGN_NAME "gain"
#define ICHNAEA_DB_CAL_RGN_NAME "cal"
#define ICHNAEA_DB_LOG_RGN_NAME "log"

extern const struct fal_flash_dev fdb_nor_flash0;
#define FAL_FLASH_DEV_TABLE \
  {                         \
    &fdb_nor_flash0,        \
  }

    /*                  partition,        device,     start,    length     */
#define FAL_PART_TABLE                                                         \
{                                                                              \
  { FAL_PART_MAGIC_WORD, ICHNAEA_DB_GAIN_RGN_NAME, ICHNAEA_DB_FLASH_DEV_NAME,           0, 1024 * 1024, 0 }, \
  { FAL_PART_MAGIC_WORD,  ICHNAEA_DB_CAL_RGN_NAME, ICHNAEA_DB_FLASH_DEV_NAME, 1024 * 1024, 1024 * 1024, 0 }, \
  { FAL_PART_MAGIC_WORD,  ICHNAEA_DB_LOG_RGN_NAME, ICHNAEA_DB_FLASH_DEV_NAME, 2048 * 1024, 2048 * 1024, 0 }, \
}

#ifdef __cplusplus
}
#endif
#endif  /* ICHNAEA_FAL_CFG_H */
