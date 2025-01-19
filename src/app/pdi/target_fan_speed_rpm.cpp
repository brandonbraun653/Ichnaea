#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/pdi/target_fan_speed_rpm.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  bool setTargetFanSpeedRPM( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.targetFanSpeedRPM ) );
    return write( KEY_TARGET_FAN_SPEED_RPM, &value, sizeof( value ) ) == sizeof( value );
  }

  float getTargetFanSpeedRPM()
  {
    float value = 0.0f;
    read( KEY_TARGET_FAN_SPEED_RPM, &value, sizeof( value ) );
    return value;
  }

  static void onWrite__target_fan_speed_rpm( mb::db::KVNode &node )
  {
    ( void )node;
    App::Monitor::refreshPDIDependencies( System::Sensor::Element::FAN_SPEED );
  }

  void pdi_register_key__target_fan_speed_rpm()
  {
    using namespace App;
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    PDI::Internal::RAMCache.targetFanSpeedRPM = 200.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_TARGET_FAN_SPEED_RPM;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.targetFanSpeedRPM;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;
    node.onWrite   = VisitorFunc::create<onWrite__target_fan_speed_rpm>();

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
