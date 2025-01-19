cmake_minimum_required(VERSION 3.15)

# -----------------------------------------------------------------------------
# High level compiler settings
# -----------------------------------------------------------------------------
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_COMPILER_WORKS)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# Propagate simulator specific configuration to all targets
if( ${PICO_PLATFORM} STREQUAL "host" )
  add_compile_definitions(-DMBEDUTILS_SIMULATOR)

  # Compiles the CMSIS-DSP library such that it doesn't pull in the
  # ARM CMSIS library. This is necessary for the simulator build.
  add_compile_definitions(-D__GNUC_PYTHON__)
endif()

# -----------------------------------------------------------------------------
# Help CMake find the custom NanoPB modules
# -----------------------------------------------------------------------------
set(NANOPB_SRC_ROOT_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/lib/mbedutils/lib/nanopb")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${NANOPB_SRC_ROOT_FOLDER}/extra)

# -----------------------------------------------------------------------------
# Import Pico SDK helper files. This must be done before the project() command
# else the correct ARM compiler won't be detected.
# -----------------------------------------------------------------------------
set(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/lib/pico-sdk")
include(pico_sdk_import.cmake)
include(pico_extras_import_optional.cmake)

# -----------------------------------------------------------------------------
# Import the FreeRTOS Kernel. This must be done before the project() command
# for the RPI Pico to detect it properly. Simulator must be done later...
# -----------------------------------------------------------------------------
set(FREERTOS_KERNEL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/freertos-kernel")
set(FREERTOS_HOST_PORT_PATH "${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/Posix")
set(FREERTOS_RP2040_PORT_PATH "${FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040")

if( ${PICO_PLATFORM} STREQUAL "rp2040" )
  include(${FREERTOS_RP2040_PORT_PATH}/FreeRTOS_Kernel_import.cmake)
endif()

# -----------------------------------------------------------------------------
# Find and configure the Doxygen package
# -----------------------------------------------------------------------------
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


# *****************************************************************************
# Start of the project configuration
# *****************************************************************************
project(Ichnaea C CXX ASM)

# -----------------------------------------------------------------------------
# Shared Paths
# -----------------------------------------------------------------------------

# Location for build/test/analytics artifacts
set(ICHNAEA_ARTIFACTS_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/artifacts)
file(MAKE_DIRECTORY ${ICHNAEA_ARTIFACTS_OUTPUT_DIR})

# -----------------------------------------------------------------------------
# Configure the FreeRTOS Kernel, done after the project() command.
# -----------------------------------------------------------------------------
# Common FreeRTOS Kernel configuration
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config INTERFACE
    ${FREERTOS_KERNEL_PATH}/include
)

if( ${PICO_PLATFORM} STREQUAL "rp2040" ) # On-device FreeRTOS
  target_include_directories(freertos_config INTERFACE
    ${FREERTOS_RP2040_PORT_PATH}/include
    ${CMAKE_CURRENT_LIST_DIR}/src/integration/freertos/hw
  )

  target_link_libraries(freertos_config INTERFACE
    pico_sync_headers
    pico_platform_headers
  )
elseif( ${PICO_PLATFORM} STREQUAL "host" )  # Simulated FreeRTOS
  # Hilariously, this has to be done **after** the project() command in the host
  # configuration so that pthreads can be found correctly. *facepalm*.
  set(FREERTOS_PORT GCC_POSIX CACHE STRING "")
  set(FREERTOS_HEAP 4 CACHE STRING "")
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/freertos-kernel)

  target_include_directories(freertos_config INTERFACE
      ${FREERTOS_HOST_PORT_PATH}
      ${FREERTOS_HOST_PORT_PATH}/utils
      ${CMAKE_CURRENT_LIST_DIR}/src/integration/freertos/sim
  )
endif()

# -----------------------------------------------------------------------------
# Configure the Pico SDK, done after the project() command.
# -----------------------------------------------------------------------------
pico_sdk_init()

# -----------------------------------------------------------------------------
# Enable CMake based testing, done before importing any dependencies.
# -----------------------------------------------------------------------------
enable_testing()

# -----------------------------------------------------------------------------
# Add project dependencies
# -----------------------------------------------------------------------------
# Library components must come first
add_subdirectory(lib/cmsis-dsp)
# Extend the CMSISDSP header target to find integration dependencies
target_include_directories(CMSISDSP PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/cmsis-6/CMSIS/Core/Include
)

target_compile_definitions(CMSISDSP PUBLIC
  -D__GNUC_PYTHON__
)

add_subdirectory(lib/etl)
add_subdirectory(lib/pico-mock)
add_subdirectory(lib/mbedutils)
add_subdirectory(lib/mbedutils_freertos)
add_subdirectory(lib/mbedutils_pico)
add_subdirectory(lib/mbedutils_test)

if(${PICO_PLATFORM} STREQUAL "host")
  add_subdirectory(lib/mbedutils_sim)
endif()

# Add project source and test directories. Must be done after dependencies are added for proper CMake configuration.
add_subdirectory(src)
add_subdirectory(tests)

# Extend the mbedutils header target to find integration dependencies
target_include_directories(mbedutils_headers INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/integration/flashdb
  ${CMAKE_CURRENT_SOURCE_DIR}/src/integration/mbedutils
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/etl/include
)

# -----------------------------------------------------------------------------
# Add a custom target for running tests with coverage reporting
# -----------------------------------------------------------------------------
find_program(LCOV_PATH lcov)
find_program(GCOVR_PATH gcovr)
find_program(GENHTML_PATH genhtml)

set(ARCHIVE_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/archive)
set(COVERAGE_OUTPUT_DIR ${ARCHIVE_OUTPUT_DIR}/coverage)
set(ICHNAEA_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_custom_target(UnitTests
  COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --cyan "Building all tests..."
  COMMAND ${CMAKE_COMMAND} -E env ${CMAKE_MAKE_PROGRAM} BuildAllTests
  COMMAND mkdir -p "${ARCHIVE_OUTPUT_DIR}"
  COMMAND mkdir -p "${COVERAGE_OUTPUT_DIR}"
  COMMAND ${CMAKE_COMMAND} -E chdir ${PROJECT_BINARY_DIR} ctest --output-on-failure --no-compress-output --output-junit ${ARCHIVE_OUTPUT_DIR}/junit_results.xml
  COMMAND ${GCOVR_PATH} --root ${CMAKE_CURRENT_SOURCE_DIR} --filter ${ICHNAEA_SRC_DIR} --object-directory=${CMAKE_CURRENT_BINARY_DIR} --html-details -o ${COVERAGE_OUTPUT_DIR}/index.html
  COMMAND echo "Coverage report generated at ${COVERAGE_OUTPUT_DIR}/index.html"
  COMMAND echo "JUnit test results generated at ${ARCHIVE_OUTPUT_DIR}/junit_results.xml"
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)
