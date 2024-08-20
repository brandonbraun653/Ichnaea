/******************************************************************************
 *  File Name:
 *    sim_adc.c
 *
 *  Description:
 *    Simulated stubs for the pico-sdk hardware ADC module
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include "hardware/adc.h"

void adc_init( void )
{
}

void adc_gpio_init( uint gpio )
{
}

void adc_select_input( uint adc_input )
{
}

uint16_t adc_read( void )
{
  return 0;
}
