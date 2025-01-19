/******************************************************************************
 *  File Name:
 *    app_filter.cpp
 *
 *  Description:
 *    Filtering algorithms for the Ichnaea PDI system.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <etl/iterator.h>
#include <mbedutils/assert.hpp>
#include <src/app/app_filter.hpp>
#include <src/app/app_pdi.hpp>
#include <src/app/generated/default_filter_config.hpp>
#include <src/app/pdi/mon_12v0_voltage.hpp>
#include <src/app/pdi/mon_1v1_voltage.hpp>
#include <src/app/pdi/mon_3v3_voltage.hpp>
#include <src/app/pdi/mon_5v0_voltage.hpp>
#include <src/app/pdi/mon_fan_speed.hpp>
#include <src/app/pdi/mon_input_voltage.hpp>
#include <src/app/pdi/mon_output_current.hpp>
#include <src/app/pdi/mon_output_voltage.hpp>
#include <src/app/pdi/mon_temperature.hpp>
#include <src/app/proto/ichnaea_pdi.pb.h>
#include <src/system/system_db.hpp>

namespace App::Filter
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  void driver_init()
  {
    using namespace App::PDI;

    pdi_register_key__input_voltage_filter();
    pdi_register_key__output_current_filter();
    pdi_register_key__output_voltage_filter();
    pdi_register_key__1v1_voltage_filter();
    pdi_register_key__3v3_voltage_filter();
    pdi_register_key__5v0_voltage_filter();
    pdi_register_key__12v0_voltage_filter();
    pdi_register_key__temperature_filter();
    pdi_register_key__fan_speed_filter();
  }


  /*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/

  IIRFilter::IIRFilter()
  {
  }


  IIRFilter::~IIRFilter()
  {
  }


  bool IIRFilter::initialize( const PDI::PDIKey filter_config_key )
  {
    /*-------------------------------------------------------------------------
    Store the configuration into the class
    -------------------------------------------------------------------------*/
    auto old_config = config;
    if( PDI::read( filter_config_key, &config, sizeof( config ) ) <= 0 )
    {
      return false;
    }

    /*-------------------------------------------------------------------------
    Initialize the filter with the new configuration
    -------------------------------------------------------------------------*/
    uint8_t num_stages = etl::max( config.order / 2, 1 );

    mbed_assert( config.order > 0 && config.order <= 6 );
    mbed_assert( ETL_ARRAY_SIZE( state ) >= config.order );
    mbed_assert( ETL_ARRAY_SIZE( config.coefficients ) >= ( 5 * num_stages ) );

    if( memcmp( &old_config, &config, sizeof( config ) ) != 0 )
    {
      memset( state, 0, sizeof( state ) );
      arm_biquad_cascade_df2T_init_f32( &filter, num_stages, config.coefficients, state );
    }

    return true;
  }


  void IIRFilter::reset()
  {
    memset( state, 0, sizeof( state ) );
  }


  float IIRFilter::apply( const float input )
  {
    float ret_val = 0.0f;
    arm_biquad_cascade_df2T_f32( &filter, &input, &ret_val, 1 );
    return ret_val;
  }

}    // namespace App::Filter
