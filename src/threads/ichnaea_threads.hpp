/******************************************************************************
 *  File Name:
 *    ichnaea_threads.hpp
 *
 *  Description:
 *    Thread declarations for the Ichnaea system
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

#pragma once
#ifndef ICHNAEA_THREADS_HPP
#define ICHNAEA_THREADS_HPP

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/


namespace Threads
{
  void monitorThread( void );

  void controlThread( void );
}    // namespace Threads

#endif /* !ICHNAEA_THREADS_HPP */
