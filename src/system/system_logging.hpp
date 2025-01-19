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

  /**
   * @brief Perform any necessary steps once the system has been fully initialized
   */
  void postSequence();

  /*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/

  /**
   * @brief Sends encoded NanoPB messages to a serial channel
   */
  class RPCSink : public ::mb::logging::SinkInterface
  {
  public:
    RPCSink() : mRpcServer( nullptr ){};
    ~RPCSink() = default;

    ::mb::logging::ErrCode write( const ::mb::logging::Level level, const void *const message,
                                  const size_t length ) final override;

    ::mb::logging::ErrCode open() final override
    {
      this->initLockable();
      return ::mb::logging::ErrCode::ERR_OK;
    };

    ::mb::logging::ErrCode close() final override
    {
      return ::mb::logging::ErrCode::ERR_OK;
    };

    ::mb::logging::ErrCode flush() final override
    {
      return ::mb::logging::ErrCode::ERR_OK;
    };

    ::mb::logging::ErrCode erase() final override
    {
      return ::mb::logging::ErrCode::ERR_OK;
    };

    void read( ::mb::logging::LogReader visitor, const bool direction = false ) final override
    {
      ( void )visitor;
      ( void )direction;
    };

    /**
     * @brief Assigns the serial driver to use for logging
     *
     * @param serial Driver to attach for logging
     */
    void assignDriver( ::mb::rpc::server::Server &rpcServer );

  private:
    mbed_rpc_ConsoleMessage    msg;
    ::mb::rpc::server::Server *mRpcServer; /**< Driver for logging messages */
  };

}    // namespace Logging

#endif /* !ICHNAEA_SYSTEM_LOGGING_HPP */
