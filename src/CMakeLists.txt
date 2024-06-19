
add_subdirectory(bsp)
add_subdirectory(system)

# Shared Targets for Header Includes
add_library(Ichnaea_Headers INTERFACE)
target_include_directories(Ichnaea_Headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../)

# -----------------------------------------------------------------------------
# Pico Interface Headers
# -----------------------------------------------------------------------------
add_library(Ichnaea_PicoHeaders INTERFACE)
target_link_libraries(Ichnaea_PicoHeaders INTERFACE
  # These headers work on all platforms
  pico_stdlib_headers
  pico_stdio_headers
  pico_time_headers
  hardware_gpio_headers
  hardware_timer_headers
  hardware_uart_headers
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
  pico_stdlib
  hardware_gpio
)

# Some of the HW libraries don't work on the host target due to inline assembly or
# other HW specific code. Redirect to the mock library which has the same API.
if(${PICO_PLATFORM} STREQUAL "rp2040")
  target_link_libraries(Ichnaea_PicoLibs INTERFACE hardware_adc)
elseif(${PICO_PLATFORM} STREQUAL "host")
  # Collect all the simulator stubs for the Pico SDK
  file(GLOB PICO_SIM_STUBS ${CMAKE_CURRENT_SOURCE_DIR}/sim/*.c)
  add_library(Ichnaea_PicoStubs STATIC ${PICO_SIM_STUBS})
  target_link_libraries(Ichnaea_PicoStubs PRIVATE pico_mock_headers)

  # Link the stubs to the PicoLibs interface
  target_link_libraries(Ichnaea_PicoLibs INTERFACE Ichnaea_PicoStubs)
endif()

add_executable(IchnaeaApp main.cpp)
target_link_libraries(IchnaeaApp
  Ichnaea_Board
  Ichnaea_Headers
  Ichnaea_PicoHeaders
  Ichnaea_PicoLibs
  Ichnaea_System
)

# create map/bin/hex file etc.
pico_add_extra_outputs(IchnaeaApp)