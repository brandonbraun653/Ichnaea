# -----------------------------------------------------------------------------
# Project Libraries
# -----------------------------------------------------------------------------
add_subdirectory(bsp)
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
)
target_link_libraries(Ichnaea_Headers INTERFACE
  mbedutils_headers
)

# -----------------------------------------------------------------------------
# Pico Interface Headers
# -----------------------------------------------------------------------------
add_library(Ichnaea_PicoHeaders INTERFACE)
target_link_libraries(Ichnaea_PicoHeaders INTERFACE
  # These headers work on all platforms
  hardware_clocks_headers
  hardware_dma_headers
  hardware_gpio_headers
  hardware_irq_headers
  hardware_pwm_headers
  hardware_spi_headers
  hardware_timer_headers
  hardware_uart_headers
  pico_multicore_headers
  pico_platform_headers
  pico_stdio_headers
  pico_stdlib_headers
  pico_sync_headers
  pico_time_headers
)

# Some of the HW headers don't work on the host target due to inline assembly or
# other HW specific code. Redirect to the mock library which has the same API.
if(${PICO_PLATFORM} STREQUAL "rp2040")
  target_link_libraries(Ichnaea_PicoHeaders INTERFACE hardware_adc_headers)
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
  hardware_gpio
  hardware_irq
  hardware_pwm
  hardware_spi
  pico_multicore
  pico_platform
  pico_stdlib
  pico_sync
)

# Some of the HW libraries don't work on the host target due to inline assembly or
# other HW specific code. Redirect to the mock library which has the same API.
if(${PICO_PLATFORM} STREQUAL "rp2040")
  target_link_libraries(Ichnaea_PicoLibs INTERFACE hardware_adc)
elseif(${PICO_PLATFORM} STREQUAL "host")
  target_link_libraries(Ichnaea_PicoLibs INTERFACE Ichnaea_PicoStubs)
endif()

add_executable(IchnaeaApp ichnaea_main.cpp)
target_link_libraries(IchnaeaApp
  Ichnaea_Board
  Ichnaea_Hardware
  Ichnaea_Headers
  Ichnaea_MbedUtils
  Ichnaea_PicoHeaders
  Ichnaea_PicoLibs
  Ichnaea_System
  Ichnaea_Threads
  mbedutils_lib_core
  mbedutils_lib_hardware
  mbedutils_lib_pico
  mbedutils_logging
  mbedutils_logging_bare_metal
)
# Fix annoying linker ordering dependencies that might get introduced
target_link_options(IchnaeaApp PRIVATE -Wl,--start-group)

# create map/bin/hex file etc.
pico_add_extra_outputs(IchnaeaApp)