/******************************************************************************
 *  File Name:
 *    app_filter.hpp
 *
 *  Description:
 *    Filtering algorithms for the Ichnaea PDI system.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_FILTER_HPP
#define ICHNAEA_FILTER_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "dsp/filtering_functions.h"
#include <src/app/proto/ichnaea_pdi.pb.h>
#include <src/app/app_pdi.hpp>

namespace App::Filter
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Initialize the filter system
   */
  void driver_init();

  /*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/

  /**
   * @brief Runtime wrapper for an IIR filter
   */
  class IIRFilter
  {
  public:
    IIRFilter();
    ~IIRFilter();

    /**
     * @brief Initialize the filter from a PDI configuration key
     *
     * @param filter_config_key Key holding the filter configuration
     * @return True if successful, false otherwise
     */
    bool initialize( const PDI::PDIKey filter_config_key );

    /**
     * @brief Reset the filter to its initial state
     */
    void reset();

    /**
     * @brief Apply the filter to a new input value
     *
     * @param input New input value to filter
     * @return float Filtered output value
     */
    float apply( const float input );

  private:
    arm_biquad_cascade_df2T_instance_f32 filter;
    ichnaea_PDI_IIRFilterConfig          config;
    float                                state[ ichnaea_PDI_IIRFilterConfig_MaxFilterOrder_MAX_FILTER_ORDER ];
  };

}    // namespace App::Filter

#endif /* !ICHNAEA_FILTER_HPP */
