# -----------------------------------------------------------------------------
# Ichnaea Source Specific Configuration
# These options apply to all files in this directory and below.
# -----------------------------------------------------------------------------
add_compile_options(
  -Wall
  -Wextra
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DDEBUG)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_definitions(-DNDEBUG)
  if(${PICO_PLATFORM} STREQUAL "host")
    add_compile_options(-g) # Enable debug symbols for the simulator
  endif()
endif()

# -----------------------------------------------------------------------------
# Integration Libraries
# -----------------------------------------------------------------------------

# Directly modify the mbedutils_lib_db target to include the project's flashdb integration
target_include_directories(mbedutils_lib_db PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/integration/flashdb)

# -----------------------------------------------------------------------------
# Project Libraries
# -----------------------------------------------------------------------------
add_subdirectory(app)
add_subdirectory(bsp)
add_subdirectory(com)
add_subdirectory(hw)
add_subdirectory(integration/mbedutils)
add_subdirectory(system)
add_subdirectory(threads)

if(${PICO_PLATFORM} STREQUAL "host")
  add_subdirectory(sim)
endif()

# -----------------------------------------------------------------------------
# Project Interface Headers
# -----------------------------------------------------------------------------
add_library(Ichnaea_Headers INTERFACE)
target_include_directories(Ichnaea_Headers INTERFACE
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/lib/etl/include
  ${PROJECT_SOURCE_DIR}/lib/mbedutils/lib/nanoprintf
  ${PROJECT_SOURCE_DIR}/lib/cmsis-6/CMSIS/Core/Include
  ${PROJECT_SOURCE_DIR}/lib/cmsis-dsp/Include
)
target_link_libraries(Ichnaea_Headers INTERFACE
  mbedutils_headers
  mbedutils_lib_rpc_headers
  freertos_config
)

# -----------------------------------------------------------------------------
# Pico Interface Headers
# -----------------------------------------------------------------------------
add_library(Ichnaea_PicoHeaders INTERFACE)

# Some of the HW headers don't work on the host target due to inline assembly or
# other HW specific code. Redirect to the mock library which has the same API.
if(${PICO_PLATFORM} STREQUAL "rp2040")
  target_link_libraries(Ichnaea_PicoHeaders INTERFACE
    hardware_adc_headers
    hardware_clocks_headers
    hardware_dma_headers
    hardware_exception_headers
    hardware_gpio_headers
    hardware_irq_headers
    hardware_pwm_headers
    hardware_spi_headers
    hardware_sync_headers
    hardware_timer_headers
    hardware_uart_headers
    pico_bit_ops_headers
    pico_multicore_headers
    pico_platform_headers
    pico_stdio_headers
    pico_stdlib_headers
    pico_sync_headers
    pico_time_headers
    pico_unique_id_headers
    )
elseif(${PICO_PLATFORM} STREQUAL "host")
  target_link_libraries(Ichnaea_PicoHeaders INTERFACE pico_mock_headers)
endif()

# -----------------------------------------------------------------------------
# Ichnaea Application
# -----------------------------------------------------------------------------
add_executable(Ichnaea ichnaea_main.cpp)

# Common libraries for both HW/SIM platforms
target_link_libraries(Ichnaea PRIVATE
  CMSISDSP
  Ichnaea_Application
  Ichnaea_Board
  Ichnaea_Com
  Ichnaea_Headers
  Ichnaea_MbedUtils
  Ichnaea_PicoHeaders
  Ichnaea_System
  Ichnaea_Threads
  mbedutils_lib_core
  mbedutils_lib_db
  mbedutils_lib_freertos
  mbedutils_lib_hardware
  mbedutils_lib_nvm
  mbedutils_lib_rpc_cobs
  mbedutils_lib_rpc_nanopb
  mbedutils_lib_rpc_proto
  mbedutils_lib_rpc_server
  mbedutils_lib_system
  mbedutils_lib_thread
  mbedutils_logging
  mbedutils_logging_bare_metal
  mbedutils_logging_tsdb
)

# Swap out the HW/SIM implementations of the mbedutils interface layer
if(${PICO_PLATFORM} STREQUAL "rp2040")
  target_link_libraries(Ichnaea PRIVATE
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
    Ichnaea_Hardware
    hardware_adc
    hardware_clocks
    hardware_dma
    hardware_exception
    hardware_gpio
    hardware_irq
    hardware_pwm
    hardware_spi
    hardware_uart
    mbedutils_lib_pico
    pico_multicore
    pico_platform
    pico_stdlib
    pico_sync
    pico_unique_id
  )
elseif(${PICO_PLATFORM} STREQUAL "host")
  target_link_libraries(Ichnaea PRIVATE
    Ichnaea_Simulator
    freertos_kernel
    mbedutils_lib_sim
    mbedutils_logging_stl
  )
endif()

# Fix annoying linker ordering dependencies that might get introduced
target_link_options(Ichnaea PRIVATE -Wl,--start-group)

# Create map/bin/hex file etc.
pico_add_extra_outputs(Ichnaea)

# Use the project side linker script for the RP2040 platform, in case we've made changes
if(${PICO_PLATFORM} STREQUAL "rp2040")
pico_set_linker_script(Ichnaea ${CMAKE_CURRENT_SOURCE_DIR}/ichnaea_linker.ld)
endif()

# Dump a summary of the memory usage for embedded platforms
if( ${PICO_PLATFORM} STREQUAL "rp2040" )
target_link_options(Ichnaea PRIVATE -Wl,--print-memory-usage)
endif()

# Copy artifacts to the output directory
set(BUILD_CONTEXT_OUTPUT_DIR ${ICHNAEA_ARTIFACTS_OUTPUT_DIR}/${PICO_PLATFORM}/${CMAKE_BUILD_TYPE})

add_custom_command(TARGET Ichnaea POST_BUILD
  # Copy the compile_commands.json file to the artifacts directory
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${ICHNAEA_ARTIFACTS_OUTPUT_DIR}/compile_commands.json

  # Copy the simulator/embedded binary to the artifacts directory
  COMMAND ${CMAKE_COMMAND} -E make_directory ${BUILD_CONTEXT_OUTPUT_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:Ichnaea>
    ${BUILD_CONTEXT_OUTPUT_DIR}/Ichnaea
)
