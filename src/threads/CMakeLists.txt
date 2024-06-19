file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(Ichnaea_Threads STATIC ${SOURCES})
target_link_libraries(Ichnaea_Threads PRIVATE Ichnaea_Headers Ichnaea_PicoHeaders)