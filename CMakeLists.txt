cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_COMPILER_WORKS)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

add_compile_options(
  -Wall
  -Wno-format # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
  -Wno-unused-function # we have some for the docs that aren't called
  -Wno-maybe-uninitialized
)

# Import Pico SDK. This must be done before the project() command, else the ARM compiler won't be detected.
set(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/lib/pico-sdk")
include(pico_sdk_import.cmake)
include(pico_extras_import_optional.cmake)

# Find and configure the Doxygen package
find_package(Doxygen)
set(DOXYGEN_INPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/docs)
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)
file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})
add_custom_command(
  OUTPUT ${DOXYGEN_OUTPUT_DIR}/html/index.xml
  COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
  MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
  COMMENT "Generating Doxygen documentation"
  VERBATIM
)
add_custom_target(docs ALL DEPENDS ${DOXYGEN_OUTPUT_DIR}/html/index.xml)

# Add a custom target for generating coverage reports
find_program(LCOV_PATH lcov)
find_program(GCOVR_PATH gcovr)
find_program(GENHTML_PATH genhtml)
add_custom_target(coverage
  COMMAND ${CMAKE_MAKE_PROGRAM} IchnaeaTests
  COMMAND ${CMAKE_COMMAND} -E chdir ${PROJECT_BINARY_DIR} ctest
  COMMAND ${GCOVR_PATH} -r ${CMAKE_CURRENT_BINARY_DIR}/src --object-directory=${CMAKE_CURRENT_BINARY_DIR} --html-details -o ${CMAKE_CURRENT_BINARY_DIR}/coverage.html
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)

project(Ichnaea C CXX ASM)
pico_sdk_init()
enable_testing()
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(lib/pico-mock)
add_subdirectory(lib/mbedutils)

# Extend the mbedutils header target to find integration dependencies
target_include_directories(mbedutils_headers INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/integration/mbedutils
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/etl/include
)
