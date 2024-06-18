
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
  pico_stdlib_headers
  hardware_gpio_headers

)

# Some of the HW headers don't work on the host target due to inline assembly or
# other HW specific code. Redirect to the mock library which has the same API.
if(${PICO_PLATFORM} STREQUAL "rp2040")
  target_link_libraries(Ichnaea_PicoHeaders INTERFACE hardware_adc_headers)
elseif(${PICO_PLATFORM} STREQUAL "host")
  target_link_libraries(Ichnaea_PicoHeaders INTERFACE pico_mock_headers)
endif()

# Hack to generate a library of all the pico libraries
add_library(Ichnaea_PicoLibs INTERFACE)
target_link_libraries(Ichnaea_PicoLibs INTERFACE
  pico_stdlib
  hardware_gpio
)

if(${PICO_PLATFORM} STREQUAL "rp2040")
  target_link_libraries(Ichnaea_PicoLibs INTERFACE hardware_adc)
elseif(${PICO_PLATFORM} STREQUAL "host")
  target_link_libraries(Ichnaea_PicoLibs INTERFACE pico_mock_headers pico_mock_lib)
endif()

add_executable(IchnaeaApp main.cpp)
target_link_libraries(IchnaeaApp
  Ichnaea_Headers
  Ichnaea_Board
  Ichnaea_System
  Ichnaea_PicoLibs
)

# create map/bin/hex file etc.
pico_add_extra_outputs(IchnaeaApp)