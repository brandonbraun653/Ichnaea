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
extern "C"
{
#endif

/*-----------------------------------------------------------------------------
Using FlashDB debug logging feature
-----------------------------------------------------------------------------*/
// #define FDB_DEBUG_ENABLE

/*-----------------------------------------------------------------------------
Using Key-Value Database feature
-----------------------------------------------------------------------------*/
#define FDB_USING_KVDB
// #define FDB_KV_AUTO_UPDATE

/*-----------------------------------------------------------------------------
Using Time-Series Database feature
-----------------------------------------------------------------------------*/
#define FDB_USING_TSDB
#define FDB_USING_TIMESTAMP_64BIT

/*-----------------------------------------------------------------------------
Using flash abstraction layer
-----------------------------------------------------------------------------*/
#define FDB_WRITE_GRAN 1
#define FDB_USING_FAL_MODE
#define FAL_PART_HAS_TABLE_CFG

  /*-----------------------------------------------------------------------------
  Override the default printf function
  -----------------------------------------------------------------------------*/

  extern int ichnaea_printf( const char *format, ... );

#define FDB_PRINT( ... ) ichnaea_printf( __VA_ARGS__ )

#ifdef __cplusplus
}
#endif

#endif /* ICHNAEA_FDB_CFG_H */
