/******************************************************************************
 *  File Name:
 *    fdb_cfg.h
 *
 *  Description:
 *    FlashDB configuration header for Ichnaea
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#ifndef ICHNAEA_FDB_CFG_H
#define ICHNAEA_FDB_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------
Using Key-Value Database feature
-----------------------------------------------------------------------------*/
#define FDB_USING_KVDB
#define FDB_KV_AUTO_UPDATE

/*-----------------------------------------------------------------------------
Using Time-Series Database feature
-----------------------------------------------------------------------------*/
#define FDB_USING_TSDB

/*-----------------------------------------------------------------------------
Using flash abstraction layer
-----------------------------------------------------------------------------*/
#define FDB_WRITE_GRAN 1
#define FDB_USING_FAL_MODE
#define FAL_PART_HAS_TABLE_CFG

#define FDB_PRINT(...)

#ifdef __cplusplus
}
#endif

#endif  /* ICHNAEA_FDB_CFG_H */
