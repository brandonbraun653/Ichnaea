/******************************************************************************
 *  File Name:
 *    irq_intf.cpp
 *
 *  Description:
 *    Ichnaea integration of the mb IRQ interface
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <cstdint>
#include <mbedutils/interfaces/irq_intf.hpp>
#include <src/system/system_util.hpp>
#include "hardware/sync.h"

namespace mb::irq
{
  /*---------------------------------------------------------------------------
  Static Data
  ---------------------------------------------------------------------------*/

  static volatile uint32_t s_saved_interrupt_mask;

  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  bool in_isr()
  {
    return ::System::inISR();
  }


  void disable_interrupts()
  {
    #if defined( ICHNAEA_EMBEDDED )
    s_saved_interrupt_mask = save_and_disable_interrupts();
    #endif
  }


  void enable_interrupts()
  {
    #if defined( ICHNAEA_EMBEDDED )
    restore_interrupts( s_saved_interrupt_mask );
    #endif
  }

}    // namespace mb::irq
