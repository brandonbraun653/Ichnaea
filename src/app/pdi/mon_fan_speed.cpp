#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/generated/default_filter_config.hpp>
#include <src/app/pdi/mon_fan_speed.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  static void onWrite__config_mon_fan_speed_pct_error_oor_limit( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::FAN_SPEED );
  }

  static void onWrite__mon_filter_fan_speed( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::FAN_SPEED );
  }

  static void onWrite__config_mon_fan_speed_oor_entry_delay_ms( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::FAN_SPEED );
  }

  static void onWrite__config_mon_fan_speed_oor_exit_delay_ms( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::FAN_SPEED );
  }

  bool setMonFanSpeedFiltered( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monFanSpeedFiltered ) );
    return write( KEY_MON_FAN_SPEED_FILTERED, &value, sizeof( value ) ) == sizeof( value );
  }

  float getMonFanSpeedFiltered()
  {
    float value = 0.0f;
    read( KEY_MON_FAN_SPEED_FILTERED, &value, sizeof( value ) );
    return value;
  }

  bool setMonFanSpeedValid( bool value )
  {
    return write( KEY_MON_FAN_SPEED_VALID, &value, sizeof( value ) ) == sizeof( value );
  }

  bool getMonFanSpeedValid()
  {
    bool value = false;
    read( KEY_MON_FAN_SPEED_VALID, &value, sizeof( value ) );
    return value;
  }

  bool setMonFanSpeedPctErrorOORLimit( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monFanSpeedPctErrorOORLimit ) );
    return write( KEY_MON_FAN_SPEED_PCT_ERROR_OOR_LIMIT, &value, sizeof( value ) ) == sizeof( value );
  }

  float getMonFanSpeedPctErrorOORLimit()
  {
    float value = 0.0f;
    read( KEY_MON_FAN_SPEED_PCT_ERROR_OOR_LIMIT, &value, sizeof( value ) );
    return value;
  }

  bool setMonFanSpeedOOREntryDelayMS( uint32_t value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monFanSpeedOOREntryDelayMS ) );
    return write( KEY_MON_FAN_SPEED_OOR_ENTRY_DELAY_MS, &value, sizeof( value ) ) == sizeof( value );
  }

  uint32_t getMonFanSpeedOOREntryDelayMS()
  {
    uint32_t value = 0;
    read( KEY_MON_FAN_SPEED_OOR_ENTRY_DELAY_MS, &value, sizeof( value ) );
    return value;
  }

  bool setMonFanSpeedOORExitDelayMS( uint32_t value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monFanSpeedOORExitDelayMS ) );
    return write( KEY_MON_FAN_SPEED_OOR_EXIT_DELAY_MS, &value, sizeof( value ) ) == sizeof( value );
  }

  uint32_t getMonFanSpeedOORExitDelayMS()
  {
    uint32_t value = 0;
    read( KEY_MON_FAN_SPEED_OOR_EXIT_DELAY_MS, &value, sizeof( value ) );
    return value;
  }

  bool setMonFilterFanSpeed( ichnaea_PDI_IIRFilterConfig &config )
  {
    static_assert( sizeof( config ) == sizeof( Internal::RAMCache.monFilterFanSpeed ) );
    return write( KEY_MON_FILTER_FAN_SPEED, &config, sizeof( config ) ) == sizeof( config );
  }

  ichnaea_PDI_IIRFilterConfig getMonFilterFanSpeed()
  {
    ichnaea_PDI_IIRFilterConfig config;
    read( KEY_MON_FILTER_FAN_SPEED, &config, sizeof( config ) );
    return config;
  }

  void pdi_register_key__fan_speed_filter()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    memset( &PDI::Internal::RAMCache.monFilterFanSpeed, 0, sizeof( PDI::Internal::RAMCache.monFilterFanSpeed ) );

    PDI::Internal::RAMCache.monFilterFanSpeed.order        = DFLT_FLTR_ORDER_FAN_SPEED;
    PDI::Internal::RAMCache.monFilterFanSpeed.sampleRateMs = DFLT_FLTR_SAMPLE_RATE_FAN_SPEED_MS;

    for( size_t i = 0; i < DFLT_FLTR_COEFF_FAN_SPEED_SIZE; i++ )
    {
      PDI::Internal::RAMCache.monFilterFanSpeed.coefficients[ i ] = DFLT_FLTR_COEFF_FAN_SPEED_VAL[ i ];
    }

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_MON_FILTER_FAN_SPEED;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.monFilterFanSpeed;
    node.dataSize  = ichnaea_PDI_IIRFilterConfig_size;
    node.pbFields  = ichnaea_PDI_IIRFilterConfig_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__mon_filter_fan_speed>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__fan_speed_filtered()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    Internal::RAMCache.monFanSpeedFiltered = 0.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = KEY_MON_FAN_SPEED_FILTERED;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monFanSpeedFiltered;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_VOLATILE;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__fan_speed_valid()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    Internal::RAMCache.monFanSpeedValid = false;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = KEY_MON_FAN_SPEED_VALID;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monFanSpeedValid;
    node.dataSize  = ichnaea_PDI_BooleanConfiguration_size;
    node.pbFields  = ichnaea_PDI_BooleanConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_VOLATILE;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__config_mon_fan_speed_pct_error_oor_limit()
  {
    using namespace mb::db;
    using namespace System::Database;

    Internal::RAMCache.monFanSpeedPctErrorOORLimit = 0.05f;

    KVNode node;
    node.hashKey   = KEY_MON_FAN_SPEED_PCT_ERROR_OOR_LIMIT;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monFanSpeedPctErrorOORLimit;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__config_mon_fan_speed_pct_error_oor_limit>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__config_mon_fan_speed_oor_entry_delay_ms()
  {
    using namespace mb::db;
    using namespace System::Database;

    Internal::RAMCache.monFanSpeedOOREntryDelayMS = 1000;

    KVNode node;
    node.hashKey   = KEY_MON_FAN_SPEED_OOR_ENTRY_DELAY_MS;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monFanSpeedOOREntryDelayMS;
    node.dataSize  = ichnaea_PDI_Uint32Configuration_size;
    node.pbFields  = ichnaea_PDI_Uint32Configuration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__config_mon_fan_speed_oor_entry_delay_ms>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__config_mon_fan_speed_oor_exit_delay_ms()
  {
    using namespace mb::db;
    using namespace System::Database;

    Internal::RAMCache.monFanSpeedOORExitDelayMS = 100;

    KVNode node;
    node.hashKey   = KEY_MON_FAN_SPEED_OOR_EXIT_DELAY_MS;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monFanSpeedOORExitDelayMS;
    node.dataSize  = ichnaea_PDI_Uint32Configuration_size;
    node.pbFields  = ichnaea_PDI_Uint32Configuration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__config_mon_fan_speed_oor_exit_delay_ms>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
