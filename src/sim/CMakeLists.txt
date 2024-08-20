# Collect all the simulator stubs for the Pico SDK
file(GLOB PICO_SIM_STUBS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
add_library(Ichnaea_PicoStubs STATIC ${PICO_SIM_STUBS})
target_link_libraries(Ichnaea_PicoStubs PRIVATE pico_mock_headers)