#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/generated/default_filter_config.hpp>
#include <src/app/pdi/mon_temperature.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  static void onWrite__mon_filter_temperature( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::BOARD_TEMP_0 );
  }

  static void onWrite__config_mon_temperature_oor_entry_delay_ms( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::BOARD_TEMP_0 );
  }

  static void onWrite__config_mon_temperature_oor_exit_delay_ms( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::BOARD_TEMP_0 );
  }

  bool setMonTemperatureFiltered( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monTemperatureFiltered ) );
    return write( KEY_MON_TEMPERATURE_FILTERED, &value, sizeof( value ) ) == sizeof( value );
  }

  float getMonTemperatureFiltered()
  {
    float value = 0.0f;
    read( KEY_MON_TEMPERATURE_FILTERED, &value, sizeof( value ) );
    return value;
  }

  bool setMonTemperatureValid( bool value )
  {
    return write( KEY_MON_TEMPERATURE_VALID, &value, sizeof( value ) ) == sizeof( value );
  }

  bool getMonTemperatureValid()
  {
    bool value = false;
    read( KEY_MON_TEMPERATURE_VALID, &value, sizeof( value ) );
    return value;
  }

  bool setMonTemperatureOOREntryDelayMS( uint32_t value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monTemperatureOOREntryDelayMS ) );
    return write( KEY_MON_TEMPERATURE_OOR_ENTRY_DELAY_MS, &value, sizeof( value ) ) == sizeof( value );
  }

  uint32_t getMonTemperatureOOREntryDelayMS()
  {
    uint32_t value = 0;
    read( KEY_MON_TEMPERATURE_OOR_ENTRY_DELAY_MS, &value, sizeof( value ) );
    return value;
  }

  bool setMonTemperatureOORExitDelayMS( uint32_t value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monTemperatureOORExitDelayMS ) );
    return write( KEY_MON_TEMPERATURE_OOR_EXIT_DELAY_MS, &value, sizeof( value ) ) == sizeof( value );
  }

  uint32_t getMonTemperatureOORExitDelayMS()
  {
    uint32_t value = 0;
    read( KEY_MON_TEMPERATURE_OOR_EXIT_DELAY_MS, &value, sizeof( value ) );
    return value;
  }

  bool setMonFilterTemperature( ichnaea_PDI_IIRFilterConfig &config )
  {
    return write( KEY_MON_FILTER_TEMPERATURE, &config, sizeof( config ) ) == sizeof( config );
  }

  ichnaea_PDI_IIRFilterConfig getMonFilterTemperature()
  {
    ichnaea_PDI_IIRFilterConfig config;
    read( KEY_MON_FILTER_TEMPERATURE, &config, sizeof( config ) );
    return config;
  }

  void pdi_register_key__temperature_filter()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    memset( &PDI::Internal::RAMCache.monFilterTemperature, 0, sizeof( PDI::Internal::RAMCache.monFilterTemperature ) );

    PDI::Internal::RAMCache.monFilterTemperature.order        = DFLT_FLTR_ORDER_TEMPERATURE;
    PDI::Internal::RAMCache.monFilterTemperature.sampleRateMs = DFLT_FLTR_SAMPLE_RATE_TEMPERATURE_MS;

    for( size_t i = 0; i < DFLT_FLTR_COEFF_TEMPERATURE_SIZE; i++ )
    {
      PDI::Internal::RAMCache.monFilterTemperature.coefficients[ i ] = DFLT_FLTR_COEFF_TEMPERATURE_VAL[ i ];
    }

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_MON_FILTER_TEMPERATURE;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.monFilterTemperature;
    node.dataSize  = ichnaea_PDI_IIRFilterConfig_size;
    node.pbFields  = ichnaea_PDI_IIRFilterConfig_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__mon_filter_temperature>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__temperature_filtered()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    Internal::RAMCache.monTemperatureFiltered = 0.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = KEY_MON_TEMPERATURE_FILTERED;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monTemperatureFiltered;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_VOLATILE;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__temperature_valid()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    Internal::RAMCache.monTemperatureValid = false;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = KEY_MON_TEMPERATURE_VALID;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monTemperatureValid;
    node.dataSize  = ichnaea_PDI_BooleanConfiguration_size;
    node.pbFields  = ichnaea_PDI_BooleanConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_VOLATILE;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__config_mon_temperature_oor_entry_delay_ms()
  {
    using namespace mb::db;
    using namespace System::Database;

    Internal::RAMCache.monTemperatureOOREntryDelayMS = 100;

    KVNode node;
    node.hashKey   = KEY_MON_TEMPERATURE_OOR_ENTRY_DELAY_MS;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monTemperatureOOREntryDelayMS;
    node.dataSize  = ichnaea_PDI_Uint32Configuration_size;
    node.pbFields  = ichnaea_PDI_Uint32Configuration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__config_mon_temperature_oor_entry_delay_ms>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__config_mon_temperature_oor_exit_delay_ms()
  {
    using namespace mb::db;
    using namespace System::Database;

    Internal::RAMCache.monTemperatureOORExitDelayMS = 100;

    KVNode node;
    node.hashKey   = KEY_MON_TEMPERATURE_OOR_EXIT_DELAY_MS;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monTemperatureOORExitDelayMS;
    node.dataSize  = ichnaea_PDI_Uint32Configuration_size;
    node.pbFields  = ichnaea_PDI_Uint32Configuration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__config_mon_temperature_oor_exit_delay_ms>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
