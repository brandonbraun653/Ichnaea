/******************************************************************************
 *  File Name:
 *    system_util.cpp
 *
 *  Description:
 *    Ichnaea system utility functions and helpers
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "pico/unique_id.h"
#include <src/bsp/board_map.hpp>
#include <src/system/system_util.hpp>
#include <etl/crc32.h>
#include <mbedutils/util.hpp>

namespace System
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  bool inISR()
  {
    #if defined( ICHNAEA_EMBEDDED )
    uint32_t result;

    __asm volatile ("MRS %0, ipsr" : "=r" (result) );
    return(result != 0);
    #else
    return false;
    #endif  /* ICHNAEA_EMBEDDED */
  }


  uint32_t identity()
  {
    /*-------------------------------------------------------------------------
    If the unique ID has already been generated, just return it.
    -------------------------------------------------------------------------*/
    static uint32_t unique_id = 0;
    if( unique_id != 0 )
    {
      return unique_id;
    }

    /*-------------------------------------------------------------------------
    Get the unique board ID (64 bits NOR flash id)
    -------------------------------------------------------------------------*/
    pico_unique_board_id_t board_data;
    #if defined( ICHNAEA_EMBEDDED )
    pico_get_unique_board_id( &board_data );
    #else
    memset( &board_data, 0, sizeof( pico_unique_board_id_t ) );
    #endif

    /*-------------------------------------------------------------------------
    Generate a 32-bit hash to serve as the actual unique ID. This is done
    because 32-bits can be easily manipulated by the host system. 64 bits
    creates a lot of awkward data handling logic.
    -------------------------------------------------------------------------*/
    etl::crc32 calculator;
    calculator.reset();
    for( auto &byte : board_data.id )
    {
      calculator.add( byte );
    }

    unique_id = calculator.value();
    return unique_id;
  }
}  // namespace System
