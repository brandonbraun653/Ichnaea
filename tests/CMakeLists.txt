project(IchnaeaTesting)

# Import CppUTest
add_subdirectory(../lib/cpputest ${CMAKE_CURRENT_BINARY_DIR}/cpputest)

# Add the Ichnaea source code as a library compiled with coverage flags
add_library(IchnaeaCoverageLibs STATIC
  ${PROJECT_SOURCE_DIR}/../src/bsp/board_map.cpp
  ${PROJECT_SOURCE_DIR}/../src/system/panic_handlers.cpp
  ${PROJECT_SOURCE_DIR}/../src/system/system_control.cpp
  ${PROJECT_SOURCE_DIR}/../src/system/system_error.cpp
)
target_compile_options(IchnaeaCoverageLibs PRIVATE -fprofile-arcs -ftest-coverage --coverage -g -O0)
target_link_options(IchnaeaCoverageLibs PRIVATE -fprofile-arcs -ftest-coverage)
target_link_libraries(IchnaeaCoverageLibs PRIVATE Ichnaea_PicoLibs gcov)
target_include_directories(IchnaeaCoverageLibs PRIVATE
  ${PROJECT_SOURCE_DIR}/../
)

# Add test drivers
add_library(Ichnaea_SystemTests STATIC
  ${PROJECT_SOURCE_DIR}/src/system/test_system_error.cpp
)
target_link_libraries(Ichnaea_SystemTests PRIVATE Ichnaea_Headers CppUTest)

add_executable(IchnaeaTests main_test_entry.cpp)
target_include_directories(IchnaeaTests PRIVATE ../lib/cpputest/include)
target_link_libraries(IchnaeaTests PRIVATE
  CppUTest
  CppUTestExt
  gcov
  Ichnaea_PicoLibs
  Ichnaea_SystemTests
  IchnaeaCoverageLibs
  )

add_test(NAME IchnaeaTests COMMAND IchnaeaTests)



