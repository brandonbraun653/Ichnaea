#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/generated/default_filter_config.hpp>
#include <src/app/pdi/mon_1v1_voltage.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  static void onWrite__mon_filter_1v1_voltage( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::VMON_1V1 );
  }

  bool setMon1V1VoltageFiltered( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.mon1v1VoltageFiltered ) );
    return write( KEY_MON_1V1_VOLTAGE_FILTERED, &value, sizeof( value ) ) == sizeof( value );
  }

  float getMon1V1VoltageFiltered()
  {
    float value = 0.0f;
    read( KEY_MON_1V1_VOLTAGE_FILTERED, &value, sizeof( value ) );
    return value;
  }

  bool setMon1V1VoltageValid( bool value )
  {
    return write( KEY_MON_1V1_VOLTAGE_VALID, &value, sizeof( value ) ) == sizeof( value );
  }

  bool getMon1V1VoltageValid()
  {
    bool value = false;
    read( KEY_MON_1V1_VOLTAGE_VALID, &value, sizeof( value ) );
    return value;
  }

  bool setMonFilter1V1Voltage( ichnaea_PDI_IIRFilterConfig &config )
  {
    static_assert( sizeof( config ) == sizeof( Internal::RAMCache.monFilter1v1Voltage ) );
    return write( KEY_MON_FILTER_1V1_VOLTAGE, &config, sizeof( config ) ) == sizeof( config );
  }

  ichnaea_PDI_IIRFilterConfig getMonFilter1V1Voltage()
  {
    ichnaea_PDI_IIRFilterConfig config;
    read( KEY_MON_FILTER_1V1_VOLTAGE, &config, sizeof( config ) );
    return config;
  }

  void pdi_register_key__1v1_voltage_filter()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    memset( &PDI::Internal::RAMCache.monFilter1v1Voltage, 0, sizeof( PDI::Internal::RAMCache.monFilter1v1Voltage ) );

    PDI::Internal::RAMCache.monFilter1v1Voltage.order        = DFLT_FLTR_ORDER_1V1_VOLTAGE;
    PDI::Internal::RAMCache.monFilter1v1Voltage.sampleRateMs = DFLT_FLTR_SAMPLE_RATE_1V1_VOLTAGE_MS;

    for( size_t i = 0; i < DFLT_FLTR_COEFF_1V1_VOLTAGE_SIZE; i++ )
    {
      PDI::Internal::RAMCache.monFilter1v1Voltage.coefficients[ i ] = DFLT_FLTR_COEFF_1V1_VOLTAGE_VAL[ i ];
    }

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_MON_FILTER_1V1_VOLTAGE;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.monFilter1v1Voltage;
    node.dataSize  = ichnaea_PDI_IIRFilterConfig_size;
    node.pbFields  = ichnaea_PDI_IIRFilterConfig_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = mb::db::VisitorFunc::create<onWrite__mon_filter_1v1_voltage>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__1v1_voltage_filtered()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    Internal::RAMCache.mon1v1VoltageFiltered = 0.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = KEY_MON_1V1_VOLTAGE_FILTERED;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.mon1v1VoltageFiltered;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_VOLATILE;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }

  void pdi_register_key__1v1_voltage_valid()
  {
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    Internal::RAMCache.mon1v1VoltageValid = false;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = KEY_MON_1V1_VOLTAGE_VALID;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &Internal::RAMCache.mon1v1VoltageValid;
    node.dataSize  = ichnaea_PDI_BooleanConfiguration_size;
    node.pbFields  = ichnaea_PDI_BooleanConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_VOLATILE;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
