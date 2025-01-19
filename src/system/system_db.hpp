/******************************************************************************
 *  File Name:
 *    system_db.hpp
 *
 *  Description:
 *    Ichnaea program configuration settings interface
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_SYSTEM_DB_HPP
#define ICHNAEA_SYSTEM_DB_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/database.hpp>


namespace System::Database
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initialize the system configuration settings driver
   */
  void initialize();

  /**
   * @brief Gets the system configuration database
   *
   * @return mb::db::NvmKVDB&
   */
  mb::db::NvmKVDB &pdiDB();

  /**
   * @brief Insert a new key value pair into the PDI database.
   *
   * This method will assert on any failure to insert the key into the database. If
   * the key already exists and it's backed by NVM, the NVM data will be pulled into
   * the RAM cache.
   *
   * @param node Node descriptor to insert
   * @param dflt_data Default data to insert if the key does not exist
   * @param size Size of the default data
   */
  void pdi_insert_and_create( mb::db::KVNode &node, void *dflt_data, const size_t size );

}    // namespace System::Config

#endif /* !ICHNAEA_SYSTEM_DB_HPP */
