#include <mbedutils/assert.hpp>
#include <src/app/app_monitor.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/pdi/ltc7871.hpp>
#include <src/bsp/board_map.hpp>
#include <src/system/system_db.hpp>

namespace App::PDI
{
  bool setLTCPhaseInductorDCR( float value )
  {
    static_assert( sizeof( value ) == sizeof( Internal::RAMCache.ltcPhaseInductorDCR ) );
    return write( KEY_CONFIG_LTC_PHASE_INDUCTOR_DCR, &value, sizeof( value ) ) == sizeof( value );
  }

  float getLTCPhaseInductorDCR()
  {
    float value = 0.0f;
    read( KEY_CONFIG_LTC_PHASE_INDUCTOR_DCR, &value, sizeof( value ) );
    return value;
  }

  void pdi_register_key__config_ltc_phase_inductor_dcr()
  {
    using namespace App;
    using namespace mb::db;
    using namespace System::Database;

    // Default initialize the parameter
    PDI::Internal::RAMCache.ltcPhaseInductorDCR = BSP::getIOConfig().ltc_inductor_dcr;

    // Register the parameter with the database
    KVNode node;
    node.hashKey   = PDI::KEY_CONFIG_LTC_PHASE_INDUCTOR_DCR;
    node.writer    = KVWriter_Memcpy;
    node.reader    = KVReader_Memcpy;
    node.datacache = &PDI::Internal::RAMCache.ltcPhaseInductorDCR;
    node.dataSize  = ichnaea_PDI_FloatConfiguration_size;
    node.pbFields  = ichnaea_PDI_FloatConfiguration_fields;
    node.flags     = KV_FLAG_DEFAULT_PERSISTENT;

    pdi_insert_and_create( node, node.datacache, node.dataSize );
  }
}    // namespace App::PDI
