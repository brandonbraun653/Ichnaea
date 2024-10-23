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

#define FAL_PRINTF ichnaea_printf
#define ICHNAEA_DB_FLASH_DEV_NAME "nor_flash_0"
#define ICHNAEA_DB_PDI_RGN_NAME "pdi"
#define ICHNAEA_DB_CAL_RGN_NAME "cal"
#define ICHNAEA_DB_LOG_RGN_NAME "log"

/* PDI Region */
#define ICHNAEA_DB_PDI_RGN_START ( 0 )
#define ICHNAEA_DB_PDI_RGN_SIZE ( 1024 * 1024 )

/* CAL Region */
#define ICHNAEA_DB_CAL_RGN_START ( ICHNAEA_DB_PDI_RGN_START + ICHNAEA_DB_PDI_RGN_SIZE )
#define ICHNAEA_DB_CAL_RGN_SIZE ( 1024 * 1024 )

/* LOG Region */
#define ICHNAEA_DB_LOG_RGN_START ( ICHNAEA_DB_CAL_RGN_START + ICHNAEA_DB_CAL_RGN_SIZE )
#define ICHNAEA_DB_LOG_RGN_SIZE ( 2048 * 1024 )

extern const struct fal_flash_dev fdb_nor_flash0;
#define FAL_FLASH_DEV_TABLE \
  {                         \
    &fdb_nor_flash0,        \
  }

    /*                  partition,        device,     start,    length     */
#define FAL_PART_TABLE                                                         \
{                                                                              \
  { FAL_PART_MAGIC_WORD, ICHNAEA_DB_PDI_RGN_NAME, ICHNAEA_DB_FLASH_DEV_NAME,  ICHNAEA_DB_PDI_RGN_START, ICHNAEA_DB_PDI_RGN_SIZE, 0 }, \
  { FAL_PART_MAGIC_WORD,  ICHNAEA_DB_CAL_RGN_NAME, ICHNAEA_DB_FLASH_DEV_NAME, ICHNAEA_DB_CAL_RGN_START, ICHNAEA_DB_CAL_RGN_SIZE, 0 }, \
  { FAL_PART_MAGIC_WORD,  ICHNAEA_DB_LOG_RGN_NAME, ICHNAEA_DB_FLASH_DEV_NAME, ICHNAEA_DB_LOG_RGN_START, ICHNAEA_DB_LOG_RGN_SIZE, 0 }, \
}

#ifdef __cplusplus
}
#endif
#endif  /* ICHNAEA_FAL_CFG_H */
