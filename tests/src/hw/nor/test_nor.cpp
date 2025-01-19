/******************************************************************************
 *  File Name:
 *    test_nor.cpp
 *
 *  Description:
 *    Tests nor.cpp
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <src/hw/nor.hpp>

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTestExt/MockSupportPlugin.h"
#include "CppUTest/CommandLineTestRunner.h"


/*-----------------------------------------------------------------------------
Tests
-----------------------------------------------------------------------------*/

TEST_GROUP( SystemConfig )
{
  void setup()
  {
    mock().ignoreOtherCalls();
  }

  void teardown()
  {
    mock().checkExpectations();
    mock().clear();
  }
};

TEST( SystemConfig, Placeholder )
{
  STRCMP_EQUAL("hello", "hello");
}


int main( int argc, char **argv )
{
  return RUN_ALL_TESTS( argc, argv );
}
