#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/generated/default_filter_config.hpp>
#include <src/app/pdi/mon_output_current.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  static void onWrite__config_mon_load_overcurrent_oor_entry_delay_ms( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::IMON_LOAD );
  }

  static void onWrite__config_mon_load_overcurrent_oor_exit_delay_ms( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::IMON_LOAD );
  }

  static void onWrite__mon_filter_output_current( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::IMON_LOAD );
  }

  bool setMonOutputCurrentRaw( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monOutputCurrentRaw ) );
    return write( KEY_MON_OUTPUT_CURRENT_RAW, &value, sizeof( value ) ) == sizeof( value );
  }

  float getMonOutputCurrentRaw()
  {
    float value = 0.0f;
    read( KEY_MON_OUTPUT_CURRENT_RAW, &value, sizeof( value ) );
    return value;
  }

  bool setMonOutputCurrentFiltered( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monOutputCurrentFiltered ) );
    return write( KEY_MON_OUTPUT_CURRENT_FILTERED, &value, sizeof( value ) ) == sizeof( value );
  }

  float getMonOutputCurrentFiltered()
  {
    float value = 0.0f;
    read( KEY_MON_OUTPUT_CURRENT_FILTERED, &value, sizeof( value ) );
    return value;
  }

  bool setMonOutputCurrentValid( bool value )
  {
    return write( KEY_MON_OUTPUT_CURRENT_VALID, &value, sizeof( value ) ) == sizeof( value );
  }

  bool getMonOutputCurrentValid()
  {
    bool value = false;
    read( KEY_MON_OUTPUT_CURRENT_VALID, &value, sizeof( value ) );
    return value;
  }

  bool setMonLoadOvercurrentOOREntryDelayMS( uint32_t value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monLoadOvercurrentOOREntryDelayMS ) );
    return write( KEY_MON_LOAD_OVERCURRENT_OOR_ENTRY_DELAY_MS, &value, sizeof( value ) ) == sizeof( value );
  }

  uint32_t getMonLoadOvercurrentOOREntryDelayMS()
  {
    uint32_t value = 0;
    read( KEY_MON_LOAD_OVERCURRENT_OOR_ENTRY_DELAY_MS, &value, sizeof( value ) );
    return value;
  }

  bool setMonLoadOvercurrentOORExitDelayMS( uint32_t value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monLoadOvercurrentOORExitDelayMS ) );
    return write( KEY_MON_LOAD_OVERCURRENT_OOR_EXIT_DELAY_MS, &value, sizeof( value ) ) == sizeof( value );
  }

  uint32_t getMonLoadOvercurrentOORExitDelayMS()
  {
    uint32_t value = 0;
    read( KEY_MON_LOAD_OVERCURRENT_OOR_EXIT_DELAY_MS, &value, sizeof( value ) );
    return value;
  }

  bool setMonFilterOutputCurrent( ichnaea_PDI_IIRFilterConfig &config )
  {
    static_assert( sizeof( config ) == sizeof( Internal::RAMCache.monFilterOutputCurrent ) );
    return write( KEY_MON_FILTER_OUTPUT_CURRENT, &config, sizeof( config ) ) == sizeof( config );
  }

  ichnaea_PDI_IIRFilterConfig getMonFilterOutputCurrent()
  {
    ichnaea_PDI_IIRFilterConfig config;
    read( KEY_MON_FILTER_OUTPUT_CURRENT, &config, sizeof( config ) );
    return config;
  }

  void pdi_register_key__output_current_raw()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    Internal::RAMCache.monOutputCurrentRaw = 0.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = KEY_MON_OUTPUT_CURRENT_RAW;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monOutputCurrentRaw;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_VOLATILE;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__output_current_filter()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    memset( &PDI::Internal::RAMCache.monFilterOutputCurrent, 0, sizeof( PDI::Internal::RAMCache.monFilterOutputCurrent ) );

    PDI::Internal::RAMCache.monFilterOutputCurrent.order        = DFLT_FLTR_ORDER_OUTPUT_CURRENT;
    PDI::Internal::RAMCache.monFilterOutputCurrent.sampleRateMs = DFLT_FLTR_SAMPLE_RATE_OUTPUT_CURRENT_MS;

    for( size_t i = 0; i < DFLT_FLTR_COEFF_OUTPUT_CURRENT_SIZE; i++ )
    {
      PDI::Internal::RAMCache.monFilterOutputCurrent.coefficients[ i ] = DFLT_FLTR_COEFF_OUTPUT_CURRENT_VAL[ i ];
    }

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_MON_FILTER_OUTPUT_CURRENT;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.monFilterOutputCurrent;
    node.dataSize  = ichnaea_PDI_IIRFilterConfig_size;
    node.pbFields  = ichnaea_PDI_IIRFilterConfig_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__mon_filter_output_current>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__output_current_filtered()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    Internal::RAMCache.monOutputCurrentFiltered = 0.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = KEY_MON_OUTPUT_CURRENT_FILTERED;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monOutputCurrentFiltered;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_VOLATILE;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__output_current_valid()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    Internal::RAMCache.monOutputCurrentValid = false;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = KEY_MON_OUTPUT_CURRENT_VALID;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monOutputCurrentValid;
    node.dataSize  = ichnaea_PDI_BooleanConfiguration_size;
    node.pbFields  = ichnaea_PDI_BooleanConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_VOLATILE;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__config_mon_load_overcurrent_oor_entry_delay_ms()
  {
    using namespace mb::db;
    using namespace System::Database;

    Internal::RAMCache.monLoadOvercurrentOOREntryDelayMS = 100;

    KVNode node;
    node.hashKey   = KEY_MON_LOAD_OVERCURRENT_OOR_ENTRY_DELAY_MS;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monLoadOvercurrentOOREntryDelayMS;
    node.dataSize  = ichnaea_PDI_Uint32Configuration_size;
    node.pbFields  = ichnaea_PDI_Uint32Configuration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__config_mon_load_overcurrent_oor_entry_delay_ms>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__config_mon_load_overcurrent_oor_exit_delay_ms()
  {
    using namespace mb::db;
    using namespace System::Database;

    Internal::RAMCache.monLoadOvercurrentOORExitDelayMS = 100;

    KVNode node;
    node.hashKey   = KEY_MON_LOAD_OVERCURRENT_OOR_EXIT_DELAY_MS;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monLoadOvercurrentOORExitDelayMS;
    node.dataSize  = ichnaea_PDI_Uint32Configuration_size;
    node.pbFields  = ichnaea_PDI_Uint32Configuration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__config_mon_load_overcurrent_oor_exit_delay_ms>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
