
#include "src/system/system_error.hpp"


#include "CppUTest/TestHarness.h"
TEST_GROUP( SystemError )
{
};

TEST( SystemError, Placeholder )
{
  STRCMP_EQUAL("hello", "hello");
  Panic::powerUp();
}