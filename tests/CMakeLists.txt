project(IchnaeaTesting)

# Import CppUTest
add_subdirectory(../lib/cpputest ${CMAKE_CURRENT_BINARY_DIR}/cpputest)

# Add the Ichnaea source code as a library compiled with coverage flags
file(GLOB_RECURSE ICHNAEA_SOURCES ${PROJECT_SOURCE_DIR}/../src/*.cpp)
add_library(Ichnaea_CoverageLib STATIC ${ICHNAEA_SOURCES})
target_compile_options(Ichnaea_CoverageLib PRIVATE -fprofile-arcs -ftest-coverage --coverage -g -O0)
target_link_options(Ichnaea_CoverageLib PRIVATE -fprofile-arcs -ftest-coverage)
target_link_libraries(Ichnaea_CoverageLib PRIVATE Ichnaea_Headers pico_mock_headers gcov)
#target_include_directories(Ichnaea_CoverageLib PRIVATE ${PROJECT_SOURCE_DIR}/../)

# Add test drivers
add_library(Ichnaea_SystemTests STATIC
  ${PROJECT_SOURCE_DIR}/src/bsp/test_board_map.cpp
  ${PROJECT_SOURCE_DIR}/src/system/test_system_error.cpp
)
target_link_libraries(Ichnaea_SystemTests PRIVATE Ichnaea_Headers pico_mock_headers CppUTest)

# Add the test executable
add_executable(IchnaeaTests main_test_entry.cpp)
target_include_directories(IchnaeaTests PRIVATE ../lib/cpputest/include)
target_link_libraries(IchnaeaTests PRIVATE
  CppUTest
  CppUTestExt
  gcov
  Ichnaea_SystemTests
  Ichnaea_CoverageLib
  pico_mock_lib
  )

add_test(NAME IchnaeaTests COMMAND IchnaeaTests)

