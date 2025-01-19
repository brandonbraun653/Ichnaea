#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/generated/default_filter_config.hpp>
#include <src/app/pdi/mon_5v0_voltage.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  static void onWrite__mon_filter_5v0_voltage( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::VMON_5V0 );
  }

  bool setMon5V0VoltageFiltered( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.mon5v0VoltageFiltered ) );
    return write( KEY_MON_5V0_VOLTAGE_FILTERED, &value, sizeof( value ) ) == sizeof( value );
  }

  float getMon5V0VoltageFiltered()
  {
    float value = 0.0f;
    read( KEY_MON_5V0_VOLTAGE_FILTERED, &value, sizeof( value ) );
    return value;
  }

  bool setMon5V0VoltageValid( bool value )
  {
    return write( KEY_MON_5V0_VOLTAGE_VALID, &value, sizeof( value ) ) == sizeof( value );
  }

  bool getMon5V0VoltageValid()
  {
    bool value = false;
    read( KEY_MON_5V0_VOLTAGE_VALID, &value, sizeof( value ) );
    return value;
  }

  bool setMonFilter5V0Voltage( ichnaea_PDI_IIRFilterConfig &config )
  {
    static_assert( sizeof( config ) == sizeof( Internal::RAMCache.monFilter5v0Voltage ) );
    return write( KEY_MON_FILTER_5V0_VOLTAGE, &config, sizeof( config ) ) == sizeof( config );
  }

  ichnaea_PDI_IIRFilterConfig getMonFilter5V0Voltage()
  {
    ichnaea_PDI_IIRFilterConfig config;
    read( KEY_MON_FILTER_5V0_VOLTAGE, &config, sizeof( config ) );
    return config;
  }

  void pdi_register_key__5v0_voltage_filter()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    memset( &PDI::Internal::RAMCache.monFilter5v0Voltage, 0, sizeof( PDI::Internal::RAMCache.monFilter5v0Voltage ) );

    PDI::Internal::RAMCache.monFilter5v0Voltage.order        = DFLT_FLTR_ORDER_5V0_VOLTAGE;
    PDI::Internal::RAMCache.monFilter5v0Voltage.sampleRateMs = DFLT_FLTR_SAMPLE_RATE_5V0_VOLTAGE_MS;

    for( size_t i = 0; i < DFLT_FLTR_COEFF_5V0_VOLTAGE_SIZE; i++ )
    {
      PDI::Internal::RAMCache.monFilter5v0Voltage.coefficients[ i ] = DFLT_FLTR_COEFF_5V0_VOLTAGE_VAL[ i ];
    }

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_MON_FILTER_5V0_VOLTAGE;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.monFilter5v0Voltage;
    node.dataSize  = ichnaea_PDI_IIRFilterConfig_size;
    node.pbFields  = ichnaea_PDI_IIRFilterConfig_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__mon_filter_5v0_voltage>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__5v0_voltage_filtered()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    Internal::RAMCache.mon5v0VoltageFiltered = 0.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = KEY_MON_5V0_VOLTAGE_FILTERED;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.mon5v0VoltageFiltered;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_VOLATILE;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__5v0_voltage_valid()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    Internal::RAMCache.mon5v0VoltageValid = false;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = KEY_MON_5V0_VOLTAGE_VALID;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.mon5v0VoltageValid;
    node.dataSize  = ichnaea_PDI_BooleanConfiguration_size;
    node.pbFields  = ichnaea_PDI_BooleanConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_VOLATILE;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
