/******************************************************************************
 *  File Name:
 *    test_board_map.cpp
 *
 *  Description:
 *    Tests board_map.cpp
 *
 *  2024 | Brandon Braun | brandonbraun653@protonmail.com
 *****************************************************************************/

/*-----------------------------------------------------------------------------
Includes
-----------------------------------------------------------------------------*/
#include <src/bsp/board_map.hpp>

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"
#include "CppUTestExt/MockSupportPlugin.h"
#include "CppUTest/CommandLineTestRunner.h"


/*-----------------------------------------------------------------------------
Tests
-----------------------------------------------------------------------------*/

TEST_GROUP( BoardMap )
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

TEST( BoardMap, Placeholder )
{
  STRCMP_EQUAL("hello", "hello");
  BSP::powerUp();
}


int main(int argc, char** argv)
{
  return RUN_ALL_TESTS(argc, argv);
}
