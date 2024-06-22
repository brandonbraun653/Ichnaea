file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(Ichnaea_System STATIC ${SOURCES})
target_link_libraries(Ichnaea_System PRIVATE Ichnaea_Headers Ichnaea_PicoHeaders)