/******************************************************************************
 *  File Name:
 *    app_pdi.cpp
 *
 *  Description:
 *    Application Programmable Data Item (PDI) interface implementation
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/assert.hpp>
#include <src/app/app_pdi.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  /*---------------------------------------------------------------------------
  Public Data
  ---------------------------------------------------------------------------*/

  PDIData Internal::RAMCache; /**< RAM cache for the PDI database */

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  int read( const PDIKey key, void *data, const size_t data_size, const size_t size )
  {
    /*-------------------------------------------------------------------------
    Input Validation
    -------------------------------------------------------------------------*/
    if( data == nullptr || data_size == 0 )
    {
      return 0;
    }

    /*-------------------------------------------------------------------------
    Read the requested data
    -------------------------------------------------------------------------*/
    return System::Database::pdiDB().read( key, data, data_size, size );
  }


  int write( const PDIKey key, void *data, const size_t size )
  {
    /*-------------------------------------------------------------------------
    Input Validation
    -------------------------------------------------------------------------*/
    if( data == nullptr || size == 0 )
    {
      return 0;
    }

    /*-------------------------------------------------------------------------
    Write the requested data
    -------------------------------------------------------------------------*/
    return System::Database::pdiDB().write( key, data, size );
  }


  size_t size( const PDIKey key )
  {
    /*-------------------------------------------------------------------------
    The size is registered with the database on power up, so we can just query
    the database for the size of the requested key.
    -------------------------------------------------------------------------*/
    auto node = System::Database::pdiDB().find( key );
    if( node == nullptr )
    {
      return 0;
    }

    return node->dataSize;
  }


  void add_on_write_callback( const PDIKey key, mb::db::VisitorFunc callback )
  {
    auto node = System::Database::pdiDB().find( key );
    mbed_dbg_assert( node != nullptr );
    node->onWrite = callback;
  }

}  // namespace App::PDI
