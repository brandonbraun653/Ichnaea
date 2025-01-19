#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/generated/default_filter_config.hpp>
#include <src/app/pdi/mon_input_voltage.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  static void onWrite__mon_filter_input_voltage( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::VMON_SOLAR_INPUT );
  }

  static void onWrite__config_mon_input_voltage_oor_entry_delay_ms( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::VMON_SOLAR_INPUT );
  }

  static void onWrite__config_mon_input_voltage_oor_exit_delay_ms( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::VMON_SOLAR_INPUT );
  }

  bool setMonInputVoltageRaw( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monInputVoltageRaw ) );
    return write( KEY_MON_INPUT_VOLTAGE_RAW, &value, sizeof( value ) ) == sizeof( value );
  }

  float getMonInputVoltageRaw()
  {
    float value = 0.0f;
    read( KEY_MON_INPUT_VOLTAGE_RAW, &value, sizeof( value ) );
    return value;
  }

  bool setMonInputVoltageFiltered( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monInputVoltageFiltered ) );
    return write( KEY_MON_INPUT_VOLTAGE_FILTERED, &value, sizeof( value ) ) == sizeof( value );
  }

  float getMonInputVoltageFiltered()
  {
    float value = 0.0f;
    read( KEY_MON_INPUT_VOLTAGE_FILTERED, &value, sizeof( value ) );
    return value;
  }

  bool setMonInputVoltageValid( bool value )
  {
    return write( KEY_MON_INPUT_VOLTAGE_VALID, &value, sizeof( value ) ) == sizeof( value );
  }

  bool getMonInputVoltageValid()
  {
    bool value = false;
    read( KEY_MON_INPUT_VOLTAGE_VALID, &value, sizeof( value ) );
    return value;
  }

  bool setMonInputVoltageOOREntryDelayMS( uint32_t value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monInputVoltageOOREntryDelayMS ) );
    return write( KEY_MON_INPUT_VOLTAGE_OOR_ENTRY_DELAY_MS, &value, sizeof( value ) ) == sizeof( value );
  }

  uint32_t getMonInputVoltageOOREntryDelayMS()
  {
    uint32_t value = 0;
    read( KEY_MON_INPUT_VOLTAGE_OOR_ENTRY_DELAY_MS, &value, sizeof( value ) );
    return value;
  }

  bool setMonInputVoltageOORExitDelayMS( uint32_t value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.monInputVoltageOORExitDelayMS ) );
    return write( KEY_MON_INPUT_VOLTAGE_OOR_EXIT_DELAY_MS, &value, sizeof( value ) ) == sizeof( value );
  }

  uint32_t getMonInputVoltageOORExitDelayMS()
  {
    uint32_t value = 0;
    read( KEY_MON_INPUT_VOLTAGE_OOR_EXIT_DELAY_MS, &value, sizeof( value ) );
    return value;
  }

  bool setMonFilterInputVoltage( ichnaea_PDI_IIRFilterConfig &config )
  {
    static_assert( sizeof( config ) == sizeof( Internal::RAMCache.monFilterInputVoltage ) );
    return write( KEY_MON_FILTER_INPUT_VOLTAGE, &config, sizeof( config ) ) == sizeof( config );
  }

  ichnaea_PDI_IIRFilterConfig getMonFilterInputVoltage()
  {
    ichnaea_PDI_IIRFilterConfig config;
    read( KEY_MON_FILTER_INPUT_VOLTAGE, &config, sizeof( config ) );
    return config;
  }

  void pdi_register_key__input_voltage_raw()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    Internal::RAMCache.monInputVoltageRaw = 0.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = KEY_MON_INPUT_VOLTAGE_RAW;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monInputVoltageRaw;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_VOLATILE;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__input_voltage_filter()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    memset( &PDI::Internal::RAMCache.monFilterInputVoltage, 0, sizeof( PDI::Internal::RAMCache.monFilterInputVoltage ) );

    PDI::Internal::RAMCache.monFilterInputVoltage.order        = DFLT_FLTR_ORDER_INPUT_VOLTAGE;
    PDI::Internal::RAMCache.monFilterInputVoltage.sampleRateMs = DFLT_FLTR_SAMPLE_RATE_INPUT_VOLTAGE_MS;

    for( size_t i = 0; i < DFLT_FLTR_COEFF_INPUT_VOLTAGE_SIZE; i++ )
    {
      PDI::Internal::RAMCache.monFilterInputVoltage.coefficients[ i ] = DFLT_FLTR_COEFF_INPUT_VOLTAGE_VAL[ i ];
    }

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_MON_FILTER_INPUT_VOLTAGE;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.monFilterInputVoltage;
    node.dataSize  = ichnaea_PDI_IIRFilterConfig_size;
    node.pbFields  = ichnaea_PDI_IIRFilterConfig_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__mon_filter_input_voltage>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__input_voltage_filtered()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    Internal::RAMCache.monInputVoltageFiltered = 0.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = KEY_MON_INPUT_VOLTAGE_FILTERED;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monInputVoltageFiltered;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_VOLATILE;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__input_voltage_valid()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    Internal::RAMCache.monInputVoltageValid = false;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = KEY_MON_INPUT_VOLTAGE_VALID;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monInputVoltageValid;
    node.dataSize  = ichnaea_PDI_BooleanConfiguration_size;
    node.pbFields  = ichnaea_PDI_BooleanConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_VOLATILE;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__config_mon_input_voltage_oor_entry_delay_ms()
  {
    using namespace mb::db;
    using namespace System::Database;

    Internal::RAMCache.monInputVoltageOOREntryDelayMS = 100;

    KVNode node;
    node.hashKey   = KEY_MON_INPUT_VOLTAGE_OOR_ENTRY_DELAY_MS;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monInputVoltageOOREntryDelayMS;
    node.dataSize  = ichnaea_PDI_Uint32Configuration_size;
    node.pbFields  = ichnaea_PDI_Uint32Configuration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__config_mon_input_voltage_oor_entry_delay_ms>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__config_mon_input_voltage_oor_exit_delay_ms()
  {
    using namespace mb::db;
    using namespace System::Database;

    Internal::RAMCache.monInputVoltageOORExitDelayMS = 100;

    KVNode node;
    node.hashKey   = KEY_MON_INPUT_VOLTAGE_OOR_EXIT_DELAY_MS;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.monInputVoltageOORExitDelayMS;
    node.dataSize  = ichnaea_PDI_Uint32Configuration_size;
    node.pbFields  = ichnaea_PDI_Uint32Configuration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__config_mon_input_voltage_oor_exit_delay_ms>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
