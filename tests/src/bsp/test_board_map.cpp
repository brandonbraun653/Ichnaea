
#include "src/bsp/board_map.hpp"


#include "CppUTest/TestHarness.h"
TEST_GROUP( BoardMap )
{
};

TEST( BoardMap, Placeholder )
{
  STRCMP_EQUAL("hello", "hello");
  BSP::powerUp();
}