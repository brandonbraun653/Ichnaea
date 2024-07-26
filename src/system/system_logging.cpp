/******************************************************************************
 *  File Name:
 *    system_logging.cpp
 *
 *  Description:
 *    Ichnaea system logging implementation details
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "src/hw/uart.hpp"
#include "src/system/system_logging.hpp"
#include "src/com/ctrl_server.hpp"
#include <etl/string.h>
#include <mbedutils/assert.hpp>
#include <mbedutils/logging.hpp>
#include <mbedutils/util.hpp>

namespace Logging
{
  using namespace ::mb::logging;

  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static RPCSink                      s_rpc_sink;
  static mb::logging::SinkHandle_rPtr s_nanopb_handle;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void initialize()
  {
    using namespace mb::logging;
    using namespace mb::hw::serial;

    /*-------------------------------------------------------------------------
    Initialize the logging framework
    -------------------------------------------------------------------------*/
    mb::logging::initialize();

    /*-------------------------------------------------------------------------
    Configure a SerialSink to use the BMS UART channel
    -------------------------------------------------------------------------*/
    s_rpc_sink.assignDriver( Control::getRPCServer() );
    s_rpc_sink.logLevel = Level::LVL_TRACE;
    s_rpc_sink.enabled  = true;

    /*-------------------------------------------------------------------------
    Register the sink with the logging framework and set it as the root output
    for all log messages.
    -------------------------------------------------------------------------*/
    s_nanopb_handle = SinkHandle_rPtr( &s_rpc_sink );
    mbed_assert( ErrCode::ERR_OK == registerSink( s_nanopb_handle ) );
    mbed_assert( ErrCode::ERR_OK == setRootSink( s_nanopb_handle ) );
  }

  /*---------------------------------------------------------------------------
  NanoPB Sink
  ---------------------------------------------------------------------------*/

  RPCSink::RPCSink() : mRpcServer( nullptr )
  {
  }


  ErrCode RPCSink::open()
  {
    this->initLockable();
    return ErrCode::ERR_OK;
  }


  ErrCode RPCSink::close()
  {
    return ErrCode::ERR_OK;
  }


  ErrCode RPCSink::flush()
  {
    return ErrCode::ERR_OK;
  }


  ErrCode RPCSink::insert( const Level level, const void *const message, const size_t length )
  {
    using namespace mb::rpc::message;

    /*-------------------------------------------------------------------------
    Check to see if we should even write
    -------------------------------------------------------------------------*/
    if( !enabled || ( level < logLevel ) || !message || !length )
    {
      return ErrCode::ERR_FAIL;
    }

    /*-------------------------------------------------------------------------
    Build up the new messages
    -------------------------------------------------------------------------*/
    mb::rpc::SeqId          seq          = mb::rpc::message::next_seq_id();
    size_t                  byte_offset  = 0;
    uint8_t                 frame_number = 0;
    const uint8_t           max_frames   = ( length / sizeof( msg.data ) ) + 1u;
    const uint8_t          *p_usr_data   = reinterpret_cast<const uint8_t *>( message );

    clear_struct( msg );

    while( frame_number < max_frames )
    {
      const size_t remaining  = length - byte_offset;
      const size_t chunk_size = std::min<size_t>( sizeof( msg.data.bytes ), remaining );

      /*-----------------------------------------------------------------------
      Construct the message
      -----------------------------------------------------------------------*/
      msg.header.version = mbed_rpc_BuiltinMessageVersion_MSG_VER_CONSOLE;
      msg.header.msgId   = mbed_rpc_BuiltinMessage_MSG_CONSOLE;
      msg.header.seqId   = seq;
      msg.header.svcId   = 0;
      msg.this_frame     = frame_number;
      msg.total_frames   = max_frames;
      msg.data.size      = chunk_size;
      memcpy( msg.data.bytes, p_usr_data + byte_offset, chunk_size );

      /*-----------------------------------------------------------------------
      Encode the data and ship it
      -----------------------------------------------------------------------*/
      if( !mRpcServer->publishMessage( msg.header.msgId, &msg ) )
      {
        return ErrCode::ERR_FAIL;
      }

      byte_offset += chunk_size;
      frame_number++;
    }

    return ErrCode::ERR_OK;
  }


  void RPCSink::assignDriver( ::mb::rpc::server::Server &server )
  {
    mRpcServer = &server;
  }
}    // namespace Logging
