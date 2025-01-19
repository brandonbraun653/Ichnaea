#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/generated/default_filter_config.hpp>
#include <src/app/pdi/mon_output_voltage.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  static void onWrite__mon_filter_output_voltage( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::VMON_LOAD );
  }

  static void onWrite__config_mon_load_voltage_pct_error_oor_limit( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::VMON_LOAD );
  }

  static void onWrite__config_mon_load_voltage_pct_error_oor_exit_delay_ms( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::VMON_LOAD );
  }

  static void onWrite__config_mon_load_voltage_pct_error_oor_entry_delay_ms( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::VMON_LOAD );
  }

  bool setMonOutputVoltageRaw( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monOutputVoltageRaw ) );
    return write( KEY_MON_OUTPUT_VOLTAGE_RAW, &value, sizeof( value ) ) == sizeof( value );
  }

  float getMonOutputVoltageRaw()
  {
    float value = 0.0f;
    read( KEY_MON_OUTPUT_VOLTAGE_RAW, &value, sizeof( value ) );
    return value;
  }

  bool setMonOutputVoltageFiltered( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monOutputVoltageFiltered ) );
    return write( KEY_MON_OUTPUT_VOLTAGE_FILTERED, &value, sizeof( value ) ) == sizeof( value );
  }

  float getMonOutputVoltageFiltered()
  {
    float value = 0.0f;
    read( KEY_MON_OUTPUT_VOLTAGE_FILTERED, &value, sizeof( value ) );
    return value;
  }

  bool setMonOutputVoltageValid( bool value )
  {
    return write( KEY_MON_OUTPUT_VOLTAGE_VALID, &value, sizeof( value ) ) == sizeof( value );
  }

  bool getMonOutputVoltageValid()
  {
    bool value = false;
    read( KEY_MON_OUTPUT_VOLTAGE_VALID, &value, sizeof( value ) );
    return value;
  }

  bool setMonLoadVoltagePctErrorOORLimit( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monLoadVoltagePctErrorOORLimit ) );
    return write( KEY_MON_LOAD_VOLTAGE_PCT_ERROR_OOR_LIMIT, &value, sizeof( value ) ) == sizeof( value );
  }

  float getMonLoadVoltagePctErrorOORLimit()
  {
    float value = 0.0f;
    read( KEY_MON_LOAD_VOLTAGE_PCT_ERROR_OOR_LIMIT, &value, sizeof( value ) );
    return value;
  }

  bool setMonLoadVoltagePctErrorOOREntryDelayMS( uint32_t value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monLoadVoltagePctErrorOOREntryDelayMS ) );
    return write( KEY_MON_LOAD_VOLTAGE_PCT_ERROR_OOR_ENTRY_DELAY_MS, &value, sizeof( value ) ) == sizeof( value );
  }

  uint32_t getMonLoadVoltagePctErrorOOREntryDelayMS()
  {
    uint32_t value = 0;
    read( KEY_MON_LOAD_VOLTAGE_PCT_ERROR_OOR_ENTRY_DELAY_MS, &value, sizeof( value ) );
    return value;
  }

  bool setMonLoadVoltagePctErrorOORExitDelayMS( uint32_t value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monLoadVoltagePctErrorOORExitDelayMS ) );
    return write( KEY_MON_LOAD_VOLTAGE_PCT_ERROR_OOR_EXIT_DELAY_MS, &value, sizeof( value ) ) == sizeof( value );
  }

  uint32_t getMonLoadVoltagePctErrorOORExitDelayMS()
  {
    uint32_t value = 0;
    read( KEY_MON_LOAD_VOLTAGE_PCT_ERROR_OOR_EXIT_DELAY_MS, &value, sizeof( value ) );
    return value;
  }

  bool setMonFilterOutputVoltage( ichnaea_PDI_IIRFilterConfig &config )
  {
    static_assert( sizeof( config ) == sizeof( Internal::RAMCache.monFilterOutputVoltage ) );
    return write( KEY_MON_FILTER_OUTPUT_VOLTAGE, &config, sizeof( config ) ) == sizeof( config );
  }

  ichnaea_PDI_IIRFilterConfig getMonFilterOutputVoltage()
  {
    ichnaea_PDI_IIRFilterConfig config;
    read( KEY_MON_FILTER_OUTPUT_VOLTAGE, &config, sizeof( config ) );
    return config;
  }

  void pdi_register_key__output_voltage_raw()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    Internal::RAMCache.monOutputVoltageRaw = 0.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = KEY_MON_OUTPUT_VOLTAGE_RAW;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monOutputVoltageRaw;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_VOLATILE;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__output_voltage_filter()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    memset( &PDI::Internal::RAMCache.monFilterOutputVoltage, 0, sizeof( PDI::Internal::RAMCache.monFilterOutputVoltage ) );

    PDI::Internal::RAMCache.monFilterOutputVoltage.order        = DFLT_FLTR_ORDER_OUTPUT_VOLTAGE;
    PDI::Internal::RAMCache.monFilterOutputVoltage.sampleRateMs = DFLT_FLTR_SAMPLE_RATE_OUTPUT_VOLTAGE_MS;

    for( size_t i = 0; i < DFLT_FLTR_COEFF_OUTPUT_VOLTAGE_SIZE; i++ )
    {
      PDI::Internal::RAMCache.monFilterOutputVoltage.coefficients[ i ] = DFLT_FLTR_COEFF_OUTPUT_VOLTAGE_VAL[ i ];
    }

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_MON_FILTER_OUTPUT_VOLTAGE;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.monFilterOutputVoltage;
    node.dataSize  = ichnaea_PDI_IIRFilterConfig_size;
    node.pbFields  = ichnaea_PDI_IIRFilterConfig_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__mon_filter_output_voltage>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__output_voltage_filtered()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    Internal::RAMCache.monOutputVoltageFiltered = 0.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = KEY_MON_OUTPUT_VOLTAGE_FILTERED;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monOutputVoltageFiltered;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_VOLATILE;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__output_voltage_valid()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    Internal::RAMCache.monOutputVoltageValid = false;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = KEY_MON_OUTPUT_VOLTAGE_VALID;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monOutputVoltageValid;
    node.dataSize  = ichnaea_PDI_BooleanConfiguration_size;
    node.pbFields  = ichnaea_PDI_BooleanConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_VOLATILE;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__config_mon_load_voltage_pct_error_oor_limit()
  {
    using namespace mb::db;
    using namespace System::Database;

    Internal::RAMCache.monLoadVoltagePctErrorOORLimit = 0.1f;

    KVNode node;
    node.hashKey   = KEY_MON_LOAD_VOLTAGE_PCT_ERROR_OOR_LIMIT;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monLoadVoltagePctErrorOORLimit;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__config_mon_load_voltage_pct_error_oor_limit>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__config_mon_load_voltage_pct_error_oor_entry_delay_ms()
  {
    using namespace mb::db;
    using namespace System::Database;

    Internal::RAMCache.monLoadVoltagePctErrorOOREntryDelayMS = 1000;

    KVNode node;
    node.hashKey   = KEY_MON_LOAD_VOLTAGE_PCT_ERROR_OOR_ENTRY_DELAY_MS;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monLoadVoltagePctErrorOOREntryDelayMS;
    node.dataSize  = ichnaea_PDI_Uint32Configuration_size;
    node.pbFields  = ichnaea_PDI_Uint32Configuration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__config_mon_load_voltage_pct_error_oor_entry_delay_ms>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__config_mon_load_voltage_pct_error_oor_exit_delay_ms()
  {
    using namespace mb::db;
    using namespace System::Database;

    Internal::RAMCache.monLoadVoltagePctErrorOORExitDelayMS = 100;

    KVNode node;
    node.hashKey   = KEY_MON_LOAD_VOLTAGE_PCT_ERROR_OOR_EXIT_DELAY_MS;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monLoadVoltagePctErrorOORExitDelayMS;
    node.dataSize  = ichnaea_PDI_Uint32Configuration_size;
    node.pbFields  = ichnaea_PDI_Uint32Configuration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__config_mon_load_voltage_pct_error_oor_exit_delay_ms>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
