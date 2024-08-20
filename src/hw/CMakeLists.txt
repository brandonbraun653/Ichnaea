file(GLOB HW_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(Ichnaea_Hardware STATIC ${HW_SOURCES})
target_link_libraries(Ichnaea_Hardware PRIVATE Ichnaea_Headers Ichnaea_PicoHeaders)