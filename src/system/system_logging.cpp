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
#include "mbedutils/drivers/logging/logging_sinks.hpp"
#include <src/bsp/board_map.hpp>
#include <src/com/ctrl_server.hpp>
#include <src/hw/uart.hpp>
#include <src/system/system_logging.hpp>
#include <cstdarg>
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

  /* Logging via RPC service */
  static RPCSink                      s_rpc_sink;
  static mb::logging::SinkHandle_rPtr s_nanopb_handle;

  /* Logging via Debug port */
  static mb::logging::SerialSink      s_debug_sink;
  static mb::logging::SinkHandle_rPtr s_debug_handle;

  /* Logging via NOR Flash */
  static mb::logging::TSDBSink        s_tsdb_sink;
  static mb::logging::SinkHandle_rPtr s_tsdb_handle;
  static uint32_t                     s_tsdb_read_buffer[ 512 / sizeof( uint32_t ) ];

#if defined( ICHNAEA_SIMULATOR )
  /* Logging via Console */
  static mb::logging::ConsoleSink     s_console_sink;
  static mb::logging::SinkHandle_rPtr s_console_handle;

  /* Logging via File */
  static mb::logging::STLFileSink     s_file_sink;
  static mb::logging::SinkHandle_rPtr s_file_handle;
#endif    // ICHNAEA_SIMULATOR

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
    Configure and register the RPC sink to output warnings and above.
    -------------------------------------------------------------------------*/
    s_rpc_sink.assignDriver( Control::getRPCServer() );
    s_rpc_sink.logLevel = Level::LVL_WARN;
    s_rpc_sink.enabled  = true;

    s_nanopb_handle = SinkHandle_rPtr( &s_rpc_sink );
    mbed_assert( ErrCode::ERR_OK == registerSink( s_nanopb_handle ) );

    /*-------------------------------------------------------------------------
    Configure and register the Debug sink to output everything.
    -------------------------------------------------------------------------*/
    s_debug_sink.assignDriver( HW::UART::getDriver( HW::UART::Channel::UART_DEBUG ) );
    s_debug_sink.logLevel = Level::LVL_TRACE;
    s_debug_sink.enabled  = true;

    s_debug_handle = SinkHandle_rPtr( &s_debug_sink );
    mbed_assert( ErrCode::ERR_OK == registerSink( s_debug_handle ) );

    /*-------------------------------------------------------------------------
    Configure and register the TSDB sink to log warnings and above.
    -------------------------------------------------------------------------*/
    TSDBSink::Config tsdb_cfg;
    tsdb_cfg.dev_name      = ICHNAEA_DB_FLASH_DEV_NAME;
    tsdb_cfg.part_name     = ICHNAEA_DB_LOG_RGN_NAME;
    tsdb_cfg.max_log_size  = sizeof( s_tsdb_read_buffer );
    tsdb_cfg.reader_buffer = reinterpret_cast<uint8_t *>( s_tsdb_read_buffer );

    s_tsdb_sink.configure( tsdb_cfg );
    s_tsdb_sink.logLevel = Level::LVL_WARN;
    s_tsdb_sink.enabled  = false;    // Enabled after system is fully initialized

    s_tsdb_handle = SinkHandle_rPtr( &s_tsdb_sink );
    mbed_assert( ErrCode::ERR_OK == registerSink( s_tsdb_handle ) );

    /*-------------------------------------------------------------------------
    Configure the Console and File sinks if we are in a simulator environment
    -------------------------------------------------------------------------*/
#if defined( ICHNAEA_SIMULATOR )
    s_console_sink.logLevel = Level::LVL_TRACE;
    s_console_sink.enabled  = true;

    s_console_handle = SinkHandle_rPtr( &s_console_sink );
    mbed_assert( ErrCode::ERR_OK == registerSink( s_console_handle ) );

    s_file_sink.setFile( "Ichnaea.log" );
    s_file_sink.logLevel = Level::LVL_TRACE;
    s_file_sink.enabled  = true;

    s_file_handle = SinkHandle_rPtr( &s_file_sink );
    mbed_assert( ErrCode::ERR_OK == registerSink( s_file_handle ) );
#endif    // ICHNAEA_SIMULATOR

    /*-------------------------------------------------------------------------
    Assign the root logger
    -------------------------------------------------------------------------*/
    mbed_assert( ErrCode::ERR_OK == setRootSink( s_debug_handle ) );

    /*-------------------------------------------------------------------------
    Initialize the RPC logger service
    -------------------------------------------------------------------------*/
    mb::rpc::service::logger::initialize();
    mbed_assert( mb::rpc::service::logger::bind( 0, s_tsdb_handle ) );
  }


  void postSequence()
  {
    /*-------------------------------------------------------------------------
    Enable the TSDB sink now that the system is fully initialized. There are
    some shared dependencies with FlashDB drivers that had to be initialized
    first.
    -------------------------------------------------------------------------*/
    s_tsdb_sink.enabled = true;
  }

  /*---------------------------------------------------------------------------
  RPCSink
  ---------------------------------------------------------------------------*/

  ErrCode RPCSink::write( const Level level, const void *const message, const size_t length )
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
    mb::rpc::SeqId seq          = mb::rpc::message::next_seq_id();
    size_t         byte_offset  = 0;
    uint8_t        frame_number = 0;
    const uint8_t  max_frames   = ( length / sizeof( msg.data ) ) + 1u;
    const uint8_t *p_usr_data   = reinterpret_cast<const uint8_t *>( message );

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


/**
 * @brief Custom printf redirect for integrating libraries to project loggers
 *
 * @param format  The format string
 * @param ...     Variable arguments
 * @return int    Number of bytes written
 */
extern "C" int ichnaea_printf( const char *format, ... )
{
  using namespace mb::logging;

  static etl::array<char, 512> log_buffer;

  va_list argptr;
  va_start( argptr, format );
  const int write_size = npf_vsnprintf( log_buffer.data(), log_buffer.max_size(), format, argptr );
  va_end( argptr );

  if( write_size > 0 )
  {
    LOG_DEBUG( "%s\r\n", log_buffer.data() );
    return write_size;
  }
  else
  {
    return 0;
  }
}
