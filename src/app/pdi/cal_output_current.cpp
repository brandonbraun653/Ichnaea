#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/pdi/cal_output_current.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  bool setCalOutputCurrent( ichnaea_PDI_BasicCalibration &value )
  {
    return write( KEY_CAL_OUTPUT_CURRENT, &value, sizeof( value ) ) == sizeof( value );
  }

  void getCalOutputCurrent( ichnaea_PDI_BasicCalibration &value )
  {
    read( KEY_CAL_OUTPUT_CURRENT, &value, sizeof( value ) );
  }

  void pdi_register_key_cal_output_current()
  {
    using namespace App;
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    PDI::Internal::RAMCache.calOutputCurrent           = {};
    PDI::Internal::RAMCache.calOutputCurrent.valid_min = -250.0f;
    PDI::Internal::RAMCache.calOutputCurrent.valid_max = 250.0f;
    PDI::Internal::RAMCache.calOutputCurrent.gain      = 1.0f;
    PDI::Internal::RAMCache.calOutputCurrent.offset    = 0.0f;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_CAL_OUTPUT_CURRENT;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.calOutputCurrent;
    node.dataSize  = ichnaea_PDI_BasicCalibration_size;
    node.pbFields  = ichnaea_PDI_BasicCalibration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
