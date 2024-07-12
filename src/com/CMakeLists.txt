file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(Ichnaea_Com STATIC ${SOURCES})
target_link_libraries(Ichnaea_Com PRIVATE Ichnaea_Headers Ichnaea_PicoHeaders)