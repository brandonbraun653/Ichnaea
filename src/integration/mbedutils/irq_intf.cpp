/******************************************************************************
 *  File Name:
 *    irq_intf.cpp
 *
 *  Description:
 *    Ichnaea integration of the mbedutils IRQ interface
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <mbedutils/interfaces/irq_intf.hpp>

namespace mbedutils::irq
{
  /*---------------------------------------------------------------------------
  Public Functions
  ---------------------------------------------------------------------------*/

  bool in_isr()
  {
    return false;
  }


  void disable_interrupts()
  {
  }


  void enable_interrupts()
  {
  }

}    // namespace mbedutils::irq
