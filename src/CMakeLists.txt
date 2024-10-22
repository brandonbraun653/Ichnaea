# -----------------------------------------------------------------------------
# Integration Libraries
# -----------------------------------------------------------------------------

target_include_directories(mbedutils_lib_db PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/integration/flashdb)

# -----------------------------------------------------------------------------
# Project Libraries
# -----------------------------------------------------------------------------
add_subdirectory(app)
add_subdirectory(bsp)
add_subdirectory(com)
add_subdirectory(hw)
add_subdirectory(integration/mbedutils)
add_subdirectory(sim)
add_subdirectory(system)
add_subdirectory(threads)

# -----------------------------------------------------------------------------
# Project Interface Headers
# -----------------------------------------------------------------------------
add_library(Ichnaea_Headers INTERFACE)
target_include_directories(Ichnaea_Headers INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/../
  ${CMAKE_CURRENT_SOURCE_DIR}/../lib/etl/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../lib/mbedutils/lib/nanoprintf
)
target_link_libraries(Ichnaea_Headers INTERFACE
  mbedutils_headers
  mbedutils_lib_rpc_headers

  # TESTING
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
# Pico Interface Libraries
# -----------------------------------------------------------------------------
add_library(Ichnaea_PicoLibs INTERFACE)
target_link_libraries(Ichnaea_PicoLibs INTERFACE
  # These libraries compile on all platforms
  hardware_clocks
  hardware_dma
  hardware_exception
  hardware_gpio
  hardware_irq
  hardware_pwm
  hardware_spi
  hardware_uart
  pico_multicore
  pico_platform
  pico_stdlib
  pico_sync
  pico_unique_id

  # TESTING
  freertos_config
)

# Some of the HW libraries don't work on the host target due to inline assembly or
# other HW specific code. Redirect to the mock library which has the same API.
if(${PICO_PLATFORM} STREQUAL "rp2040")
  target_link_libraries(Ichnaea_PicoLibs INTERFACE hardware_adc)
elseif(${PICO_PLATFORM} STREQUAL "host")
  target_link_libraries(Ichnaea_PicoLibs INTERFACE Ichnaea_PicoStubs)
endif()

add_executable(Ichnaea ichnaea_main.cpp)
target_link_libraries(Ichnaea
  Ichnaea_Application
  Ichnaea_Board
  Ichnaea_Com
  Ichnaea_Hardware
  Ichnaea_Headers
  Ichnaea_MbedUtils
  Ichnaea_PicoHeaders
  Ichnaea_PicoLibs
  Ichnaea_System
  Ichnaea_Threads
  mbedutils_lib_core
  mbedutils_lib_db
  mbedutils_lib_freertos
  mbedutils_lib_hardware
  mbedutils_lib_nvm
  mbedutils_lib_pico
  mbedutils_lib_rpc_cobs
  mbedutils_lib_rpc_nanopb
  mbedutils_lib_rpc_proto
  mbedutils_lib_rpc_server
  mbedutils_logging
  mbedutils_logging_bare_metal
  FreeRTOS-Kernel
  FreeRTOS-Kernel-Heap4
)
# Fix annoying linker ordering dependencies that might get introduced
target_link_options(Ichnaea PRIVATE -Wl,--start-group)

# create map/bin/hex file etc.
pico_add_extra_outputs(Ichnaea)
pico_set_linker_script(Ichnaea ${CMAKE_CURRENT_SOURCE_DIR}/ichnaea_linker.ld)

# Dump a summary of the memory usage for embedded platforms
if( ${PICO_PLATFORM} STREQUAL "rp2040" )
target_link_options(Ichnaea PRIVATE -Wl,--print-memory-usage)
endif()
