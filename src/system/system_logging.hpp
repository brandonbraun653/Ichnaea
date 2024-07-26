/******************************************************************************
 *  File Name:
 *    system_logging.hpp
 *
 *  Description:
 *    Core logging interface for the Ichanea system. Most of the logging effort
 *    is being offloaded to the mbedutls library.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_SYSTEM_LOGGING_HPP
#define ICHNAEA_SYSTEM_LOGGING_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/logging.hpp>
#include <mbedutils/rpc.hpp>

namespace Logging
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Init all the logging interfaces used in the project
   */
  void initialize();

  /*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/

  /**
   * @brief Sends encoded NanoPB messages to a serial channel
   */
  class RPCSink : public ::mb::logging::SinkInterface
  {
  public:
    RPCSink();
    ~RPCSink() = default;
    ::mb::logging::ErrCode open() final override;
    ::mb::logging::ErrCode close() final override;
    ::mb::logging::ErrCode flush() final override;
    ::mb::logging::ErrCode insert( const ::mb::logging::Level level, const void *const message, const size_t length ) final override;

    /**
     * @brief Assigns the serial driver to use for logging
     *
     * @param serial Driver to attach for logging
     */
    void assignDriver( ::mb::rpc::server::Server &rpcServer );

  private:
    mbed_rpc_ConsoleMessage msg;
    ::mb::rpc::server::Server *mRpcServer; /**< Driver for logging messages */
  };

}  // namespace

#endif  /* !ICHNAEA_SYSTEM_LOGGING_HPP */
