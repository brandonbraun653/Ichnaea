/******************************************************************************
 *  File Name:
 *    app_pdi.hpp
 *
 *  Description:
 *    Application Programmable Data Item (PDI) interface. This enumerates the
 *    configurable system data that can affect runtime behavior or record data
 *    from the system.
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_APP_PDI_HPP
#define ICHNAEA_APP_PDI_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>
#include <mbedutils/database.hpp>
#include <src/app/proto/ichnaea_pdi.pb.h>

namespace App::PDI
{
  /*---------------------------------------------------------------------------
  Enumerations
  ---------------------------------------------------------------------------*/

  /**
   * @brief Keys for accessing data stored in the PDI database
   *
   * Hover over ichnaea_PDI_ID_** for the protobuf description.
   */
  enum PDIKey : mb::db::HashKey
  {
    /*-------------------------------------------------------------------------
    General System Descriptors
    -------------------------------------------------------------------------*/
    KEY_BOOT_COUNT    = ichnaea_PDI_ID_BOOT_COUNT,
    KEY_SERIAL_NUMBER = ichnaea_PDI_ID_SERIAL_NUMBER,
    KEY_MFG_DATE      = ichnaea_PDI_ID_MFG_DATE,
    KEY_CAL_DATE      = ichnaea_PDI_ID_CAL_DATE,

    /*-------------------------------------------------------------------------
    Power System Descriptors
    -------------------------------------------------------------------------*/
    KEY_TARGET_SYSTEM_VOLTAGE_OUTPUT                = ichnaea_PDI_ID_TARGET_SYSTEM_VOLTAGE_OUTPUT,
    KEY_CONFIG_SYSTEM_VOLTAGE_OUTPUT_RATED_LIMIT    = ichnaea_PDI_ID_CONFIG_SYSTEM_VOLTAGE_OUTPUT_RATED_LIMIT,
    KEY_TARGET_SYSTEM_CURRENT_OUTPUT                = ichnaea_PDI_ID_TARGET_SYSTEM_CURRENT_OUTPUT,
    KEY_CONFIG_SYSTEM_CURRENT_OUTPUT_RATED_LIMIT    = ichnaea_PDI_ID_CONFIG_SYSTEM_CURRENT_OUTPUT_RATED_LIMIT,
    KEY_TARGET_PHASE_CURRENT_OUTPUT                 = ichnaea_PDI_ID_TARGET_PHASE_CURRENT_OUTPUT,
    KEY_CONFIG_PHASE_CURRENT_OUTPUT_RATED_LIMIT     = ichnaea_PDI_ID_CONFIG_PHASE_CURRENT_OUTPUT_RATED_LIMIT,
    KEY_CONFIG_MIN_SYSTEM_VOLTAGE_INPUT             = ichnaea_PDI_ID_CONFIG_MIN_SYSTEM_VOLTAGE_INPUT,
    KEY_CONFIG_MIN_SYSTEM_VOLTAGE_INPUT_RATED_LIMIT = ichnaea_PDI_ID_CONFIG_MIN_SYSTEM_VOLTAGE_INPUT_RATED_LIMIT,
    KEY_CONFIG_MAX_SYSTEM_VOLTAGE_INPUT             = ichnaea_PDI_ID_CONFIG_MAX_SYSTEM_VOLTAGE_INPUT,
    KEY_CONFIG_MAX_SYSTEM_VOLTAGE_INPUT_RATED_LIMIT = ichnaea_PDI_ID_CONFIG_MAX_SYSTEM_VOLTAGE_INPUT_RATED_LIMIT,
    KEY_PGOOD_MONITOR_TIMEOUT_MS                    = ichnaea_PDI_ID_CONFIG_PGOOD_MONITOR_TIMEOUT_MS,
    KEY_CONFIG_MIN_TEMP_LIMIT                       = ichnaea_PDI_ID_CONFIG_MIN_TEMP_LIMIT,
    KEY_CONFIG_MAX_TEMP_LIMIT                       = ichnaea_PDI_ID_CONFIG_MAX_TEMP_LIMIT,

    /*-------------------------------------------------------------------------
    Tunable Hardware Fields
    -------------------------------------------------------------------------*/
    KEY_CONFIG_LTC_PHASE_INDUCTOR_DCR = ichnaea_PDI_ID_CONFIG_LTC_PHASE_INDUCTOR_DCR,

    /*-------------------------------------------------------------------------
    Misc Configurations
    -------------------------------------------------------------------------*/
    KEY_TARGET_FAN_SPEED_RPM = ichnaea_PDI_ID_TARGET_FAN_SPEED_RPM,

    /*-------------------------------------------------------------------------
    Monitor Parameters
    -------------------------------------------------------------------------*/
    KEY_MON_LOAD_VOLTAGE_PCT_ERROR_OOR_LIMIT          = ichnaea_PDI_ID_CONFIG_MON_LOAD_VOLTAGE_PCT_ERROR_OOR_LIMIT,
    KEY_MON_FAN_SPEED_PCT_ERROR_OOR_LIMIT             = ichnaea_PDI_ID_CONFIG_MON_FAN_SPEED_PCT_ERROR_OOR_LIMIT,
    KEY_MON_FILTER_INPUT_VOLTAGE                      = ichnaea_PDI_ID_CONFIG_MON_FILTER_INPUT_VOLTAGE,
    KEY_MON_FILTER_OUTPUT_CURRENT                     = ichnaea_PDI_ID_CONFIG_MON_FILTER_OUTPUT_CURRENT,
    KEY_MON_FILTER_OUTPUT_VOLTAGE                     = ichnaea_PDI_ID_CONFIG_MON_FILTER_OUTPUT_VOLTAGE,
    KEY_MON_FILTER_1V1_VOLTAGE                        = ichnaea_PDI_ID_CONFIG_MON_FILTER_1V1_VOLTAGE,
    KEY_MON_FILTER_3V3_VOLTAGE                        = ichnaea_PDI_ID_CONFIG_MON_FILTER_3V3_VOLTAGE,
    KEY_MON_FILTER_5V0_VOLTAGE                        = ichnaea_PDI_ID_CONFIG_MON_FILTER_5V0_VOLTAGE,
    KEY_MON_FILTER_12V0_VOLTAGE                       = ichnaea_PDI_ID_CONFIG_MON_FILTER_12V0_VOLTAGE,
    KEY_MON_FILTER_TEMPERATURE                        = ichnaea_PDI_ID_CONFIG_MON_FILTER_TEMPERATURE,
    KEY_MON_FILTER_FAN_SPEED                          = ichnaea_PDI_ID_CONFIG_MON_FILTER_FAN_SPEED,
    KEY_MON_INPUT_VOLTAGE_RAW                         = ichnaea_PDI_ID_MON_INPUT_VOLTAGE_RAW,
    KEY_MON_INPUT_VOLTAGE_FILTERED                    = ichnaea_PDI_ID_MON_INPUT_VOLTAGE_FILTERED,
    KEY_MON_OUTPUT_CURRENT_RAW                        = ichnaea_PDI_ID_MON_OUTPUT_CURRENT_RAW,
    KEY_MON_OUTPUT_CURRENT_FILTERED                   = ichnaea_PDI_ID_MON_OUTPUT_CURRENT_FILTERED,
    KEY_MON_OUTPUT_VOLTAGE_RAW                        = ichnaea_PDI_ID_MON_OUTPUT_VOLTAGE_RAW,
    KEY_MON_OUTPUT_VOLTAGE_FILTERED                   = ichnaea_PDI_ID_MON_OUTPUT_VOLTAGE_FILTERED,
    KEY_MON_1V1_VOLTAGE_FILTERED                      = ichnaea_PDI_ID_MON_1V1_VOLTAGE_FILTERED,
    KEY_MON_3V3_VOLTAGE_FILTERED                      = ichnaea_PDI_ID_MON_3V3_VOLTAGE_FILTERED,
    KEY_MON_5V0_VOLTAGE_FILTERED                      = ichnaea_PDI_ID_MON_5V0_VOLTAGE_FILTERED,
    KEY_MON_12V0_VOLTAGE_FILTERED                     = ichnaea_PDI_ID_MON_12V0_VOLTAGE_FILTERED,
    KEY_MON_TEMPERATURE_FILTERED                      = ichnaea_PDI_ID_MON_TEMPERATURE_FILTERED,
    KEY_MON_FAN_SPEED_FILTERED                        = ichnaea_PDI_ID_MON_FAN_SPEED_FILTERED,
    KEY_MON_INPUT_VOLTAGE_OOR_ENTRY_DELAY_MS          = ichnaea_PDI_ID_CONFIG_MON_INPUT_VOLTAGE_OOR_ENTRY_DELAY_MS,
    KEY_MON_INPUT_VOLTAGE_OOR_EXIT_DELAY_MS           = ichnaea_PDI_ID_CONFIG_MON_INPUT_VOLTAGE_OOR_EXIT_DELAY_MS,
    KEY_MON_LOAD_OVERCURRENT_OOR_ENTRY_DELAY_MS       = ichnaea_PDI_ID_CONFIG_MON_LOAD_OVERCURRENT_OOR_ENTRY_DELAY_MS,
    KEY_MON_LOAD_OVERCURRENT_OOR_EXIT_DELAY_MS        = ichnaea_PDI_ID_CONFIG_MON_LOAD_OVERCURRENT_OOR_EXIT_DELAY_MS,
    KEY_MON_LOAD_VOLTAGE_PCT_ERROR_OOR_ENTRY_DELAY_MS = ichnaea_PDI_ID_CONFIG_MON_LOAD_VOLTAGE_PCT_ERROR_OOR_ENTRY_DELAY_MS,
    KEY_MON_LOAD_VOLTAGE_PCT_ERROR_OOR_EXIT_DELAY_MS  = ichnaea_PDI_ID_CONFIG_MON_LOAD_VOLTAGE_PCT_ERROR_OOR_EXIT_DELAY_MS,
    KEY_MON_FAN_SPEED_OOR_ENTRY_DELAY_MS              = ichnaea_PDI_ID_CONFIG_MON_FAN_SPEED_OOR_ENTRY_DELAY_MS,
    KEY_MON_FAN_SPEED_OOR_EXIT_DELAY_MS               = ichnaea_PDI_ID_CONFIG_MON_FAN_SPEED_OOR_EXIT_DELAY_MS,
    KEY_MON_TEMPERATURE_OOR_ENTRY_DELAY_MS            = ichnaea_PDI_ID_CONFIG_MON_TEMPERATURE_OOR_ENTRY_DELAY_MS,
    KEY_MON_TEMPERATURE_OOR_EXIT_DELAY_MS             = ichnaea_PDI_ID_CONFIG_MON_TEMPERATURE_OOR_EXIT_DELAY_MS,
    KEY_MON_INPUT_VOLTAGE_VALID                       = ichnaea_PDI_ID_MON_INPUT_VOLTAGE_VALID,
    KEY_MON_OUTPUT_CURRENT_VALID                      = ichnaea_PDI_ID_MON_OUTPUT_CURRENT_VALID,
    KEY_MON_OUTPUT_VOLTAGE_VALID                      = ichnaea_PDI_ID_MON_OUTPUT_VOLTAGE_VALID,
    KEY_MON_1V1_VOLTAGE_VALID                         = ichnaea_PDI_ID_MON_1V1_VOLTAGE_VALID,
    KEY_MON_3V3_VOLTAGE_VALID                         = ichnaea_PDI_ID_MON_3V3_VOLTAGE_VALID,
    KEY_MON_5V0_VOLTAGE_VALID                         = ichnaea_PDI_ID_MON_5V0_VOLTAGE_VALID,
    KEY_MON_12V0_VOLTAGE_VALID                        = ichnaea_PDI_ID_MON_12V0_VOLTAGE_VALID,
    KEY_MON_TEMPERATURE_VALID                         = ichnaea_PDI_ID_MON_TEMPERATURE_VALID,
    KEY_MON_FAN_SPEED_VALID                           = ichnaea_PDI_ID_MON_FAN_SPEED_VALID,

    /*-------------------------------------------------------------------------
    Calibration Data
    -------------------------------------------------------------------------*/
    KEY_CAL_OUTPUT_CURRENT = ichnaea_PDI_ID_CONFIG_CAL_OUTPUT_CURRENT,
  };

  /*---------------------------------------------------------------------------
  Structures
  ---------------------------------------------------------------------------*/

  /**
   * @brief A RAM cache backing for the PDI database.
   *
   * Order does not matter here, but there is a 1:1 relationship between
   * the keys, the data items, and the database entry descriptors.
   */
  struct PDIData
  {
    /*-------------------------------------------------------------------------
    General System Descriptors
    -------------------------------------------------------------------------*/
    uint32_t bootCount; /**< KEY_BOOT_COUNT */

    /*-------------------------------------------------------------------------
    Power System Descriptors
    -------------------------------------------------------------------------*/
    float    targetSystemVoltageOutput;       /**< KEY_TARGET_SYSTEM_VOLTAGE_OUTPUT */
    float    systemVoltageOutputRatedLimit;   /**< KEY_CONFIG_SYSTEM_VOLTAGE_OUTPUT_RATED_LIMIT */
    float    targetSystemCurrentOutput;       /**< KEY_TARGET_SYSTEM_CURRENT_OUTPUT */
    float    systemCurrentOutputRatedLimit;   /**< KEY_CONFIG_SYSTEM_CURRENT_OUTPUT_RATED_LIMIT */
    float    targetPhaseCurrentOutput;        /**< KEY_TARGET_PHASE_CURRENT_OUTPUT */
    float    phaseCurrentOutputRatedLimit;    /**< KEY_CONFIG_PHASE_CURRENT_OUTPUT_RATED_LIMIT */
    float    minSystemVoltageInput;           /**< KEY_CONFIG_MIN_SYSTEM_VOLTAGE_INPUT */
    float    minSystemVoltageInputRatedLimit; /**< KEY_CONFIG_MIN_SYSTEM_VOLTAGE_INPUT_RATED_LIMIT */
    float    maxSystemVoltageInput;           /**< KEY_CONFIG_MAX_SYSTEM_VOLTAGE_INPUT */
    float    maxSystemVoltageInputRatedLimit; /**< KEY_CONFIG_MAX_SYSTEM_VOLTAGE_INPUT_RATED_LIMIT */
    float    systemVoltageInputHysteresis;    /**< KEY_CONFIG_SYSTEM_VOLTAGE_INPUT_HYSTERESIS */
    uint32_t pgoodMonitorTimeoutMS;           /**< KEY_PGOOD_MONITOR_TIMEOUT_MS */
    float    configMinTempLimit;              /**< KEY_CONFIG_MIN_TEMP_LIMIT */
    float    configMaxTempLimit;              /**< KEY_CONFIG_MAX_TEMP_LIMIT */

    /*-------------------------------------------------------------------------
    Tunable Hardware Fields
    -------------------------------------------------------------------------*/
    float ltcPhaseInductorDCR; /**< KEY_CONFIG_LTC_PHASE_INDUCTOR_DCR */

    /*-------------------------------------------------------------------------
    Misc Configurations
    -------------------------------------------------------------------------*/
    float targetFanSpeedRPM; /**< KEY_TARGET_FAN_SPEED_RPM */

    /*-------------------------------------------------------------------------
    Monitor Parameters
    -------------------------------------------------------------------------*/
    float    monLoadVoltagePctErrorOORLimit;        /**< KEY_MON_LOAD_VOLTAGE_PCT_ERROR_OOR_LIMIT */
    float    monFanSpeedPctErrorOORLimit;           /**< KEY_MON_FAN_SPEED_PCT_ERROR_OOR_LIMIT */
    float    monTemperatureOORThreshold;            /**< KEY_MON_TEMPERATURE_OOR_THRESHOLD */
    uint32_t monInputVoltageOOREntryDelayMS;        /**< KEY_MON_INPUT_VOLTAGE_OOR_ENTRY_DELAY_MS */
    uint32_t monInputVoltageOORExitDelayMS;         /**< KEY_MON_INPUT_VOLTAGE_OOR_EXIT_DELAY_MS */
    uint32_t monLoadOvercurrentOOREntryDelayMS;     /**< KEY_MON_LOAD_OVERCURRENT_OOR_ENTRY_DELAY_MS */
    uint32_t monLoadOvercurrentOORExitDelayMS;      /**< KEY_MON_LOAD_OVERCURRENT_OOR_EXIT_DELAY_MS */
    uint32_t monLoadVoltagePctErrorOOREntryDelayMS; /**< KEY_MON_LOAD_VOLTAGE_PCT_ERROR_OOR_ENTRY_DELAY_MS */
    uint32_t monLoadVoltagePctErrorOORExitDelayMS;  /**< KEY_MON_LOAD_VOLTAGE_PCT_ERROR_OOR_EXIT_DELAY_MS */
    uint32_t monFanSpeedOOREntryDelayMS;            /**< KEY_MON_FAN_SPEED_OOR_ENTRY_DELAY_MS */
    uint32_t monFanSpeedOORExitDelayMS;             /**< KEY_MON_FAN_SPEED_OOR_EXIT_DELAY_MS */
    uint32_t monTemperatureOOREntryDelayMS;         /**< KEY_MON_TEMPERATURE_OOR_ENTRY_DELAY_MS */
    uint32_t monTemperatureOORExitDelayMS;          /**< KEY_MON_TEMPERATURE_OOR_EXIT_DELAY_MS */

    /* Filter Configurations */
    ichnaea_PDI_IIRFilterConfig monFilterInputVoltage;  /**< KEY_MON_FILTER_INPUT_VOLTAGE */
    ichnaea_PDI_IIRFilterConfig monFilterOutputCurrent; /**< KEY_MON_FILTER_OUTPUT_CURRENT */
    ichnaea_PDI_IIRFilterConfig monFilterOutputVoltage; /**< KEY_MON_FILTER_OUTPUT_VOLTAGE */
    ichnaea_PDI_IIRFilterConfig monFilter1v1Voltage;    /**< KEY_MON_FILTER_1V1_VOLTAGE */
    ichnaea_PDI_IIRFilterConfig monFilter3v3Voltage;    /**< KEY_MON_FILTER_3V3_VOLTAGE */
    ichnaea_PDI_IIRFilterConfig monFilter5v0Voltage;    /**< KEY_MON_FILTER_5V0_VOLTAGE */
    ichnaea_PDI_IIRFilterConfig monFilter12v0Voltage;   /**< KEY_MON_FILTER_12V0_VOLTAGE */
    ichnaea_PDI_IIRFilterConfig monFilterTemperature;   /**< KEY_MON_FILTER_TEMPERATURE */
    ichnaea_PDI_IIRFilterConfig monFilterFanSpeed;      /**< KEY_MON_FILTER_FAN_SPEED */

    /* Realtime volatile data */
    float monInputVoltageRaw;       /**< KEY_MON_INPUT_VOLTAGE_RAW */
    float monInputVoltageFiltered;  /**< KEY_MON_INPUT_VOLTAGE_FILTERED */
    float monOutputCurrentRaw;      /**< KEY_MON_OUTPUT_CURRENT_RAW */
    float monOutputCurrentFiltered; /**< KEY_MON_OUTPUT_CURRENT_FILTERED */
    float monOutputVoltageRaw;      /**< KEY_MON_OUTPUT_VOLTAGE_RAW */
    float monOutputVoltageFiltered; /**< KEY_MON_OUTPUT_VOLTAGE_FILTERED */
    float mon1v1VoltageFiltered;    /**< KEY_MON_1V1_VOLTAGE_FILTERED */
    float mon3v3VoltageFiltered;    /**< KEY_MON_3V3_VOLTAGE_FILTERED */
    float mon5v0VoltageFiltered;    /**< KEY_MON_5V0_VOLTAGE_FILTERED */
    float mon12v0VoltageFiltered;   /**< KEY_MON_12V0_VOLTAGE_FILTERED */
    float monTemperatureFiltered;   /**< KEY_MON_TEMPERATURE_FILTERED */
    float monFanSpeedFiltered;      /**< KEY_MON_FAN_SPEED_FILTERED */
    bool  monInputVoltageValid;     /**< KEY_MON_INPUT_VOLTAGE_VALID */
    bool  monOutputCurrentValid;    /**< KEY_MON_OUTPUT_CURRENT_VALID */
    bool  monOutputVoltageValid;    /**< KEY_MON_OUTPUT_VOLTAGE_VALID */
    bool  mon1v1VoltageValid;       /**< KEY_MON_1V1_VOLTAGE_VALID */
    bool  mon3v3VoltageValid;       /**< KEY_MON_3V3_VOLTAGE_VALID */
    bool  mon5v0VoltageValid;       /**< KEY_MON_5V0_VOLTAGE_VALID */
    bool  mon12v0VoltageValid;      /**< KEY_MON_12V0_VOLTAGE_VALID */
    bool  monTemperatureValid;      /**< KEY_MON_TEMPERATURE_VALID */
    bool  monFanSpeedValid;         /**< KEY_MON_FAN_SPEED_VALID */

    /*-------------------------------------------------------------------------
    Calibration Data
    -------------------------------------------------------------------------*/
    ichnaea_PDI_BasicCalibration calOutputCurrent; /**< KEY_CAL_OUTPUT_CURRENT */
  };

  /*---------------------------------------------------------------------------
  Public Data
  ---------------------------------------------------------------------------*/
  namespace Internal
  {
    extern PDIData RAMCache; /**< RAM cache for the PDI database */
  }

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  /**
   * @brief Read a data item from the PDI database
   *
   * @param key   Key to read from the database
   * @param data  Buffer to store the data
   * @param data_size Size of the data buffer
   * @param size  Size of the data item to read
   * @return int  Number of bytes read
   */
  int read( const PDIKey key, void *data, const size_t data_size, const size_t size = 0 );

  /**
   * @brief Write a data item to the PDI database
   *
   * @param key   Key to write to the database
   * @param data  Data to write
   * @param size  Size of the data to write
   * @return int  Number of bytes written
   */
  int write( const PDIKey key, void *data, const size_t size );

  /**
   * @brief Get the size of a data item in the PDI database
   *
   * @param key   Key to get the size of
   * @return size_t  Size of the data item
   */
  size_t size( const PDIKey key );

  /**
   * @brief Sets a callback to be executed when a write operation occurs on a key
   *
   * @param key     The key to register the callback for
   * @param callback The callback to execute
   */
  void add_on_write_callback( const PDIKey key, mb::db::VisitorFunc callback );

}    // namespace App::PDI

#endif /* !ICHNAEA_APP_PDI_HPP */
