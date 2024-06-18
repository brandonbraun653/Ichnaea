
add_subdirectory(bsp)
add_subdirectory(system)

# Shared Targets for Header Includes
add_library(Ichnaea_Headers INTERFACE)
target_link_libraries(Ichnaea_Headers INTERFACE pico_stdlib_headers)
target_include_directories(Ichnaea_Headers INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/../
)

# Hack to generate a library of all the pico libraries
add_library(Ichnaea_PicoLibs INTERFACE)
target_link_libraries(Ichnaea_PicoLibs INTERFACE
    pico_stdlib
    hardware_gpio
    )

if (${PICO_PLATFORM} STREQUAL "rp2040")
  target_link_libraries(Ichnaea_PicoLibs hardware_adc)
elseif(${PICO_PLATFORM} STREQUAL "host")
  #target_link_libraries(Ichnaea_PicoLibs hardware_adc)
endif()

add_executable(IchnaeaApp main.cpp )
target_link_libraries(IchnaeaApp
        Ichnaea_Headers
        Ichnaea_Board
        Ichnaea_System
        Ichnaea_PicoLibs
        pico_mock_lib
)

# create map/bin/hex file etc.
pico_add_extra_outputs(IchnaeaApp)