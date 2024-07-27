file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB PROTO_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/proto/*.c)

add_library(Ichnaea_Com STATIC
  ${SOURCES}
  ${PROTO_SOURCES}
)
target_link_libraries(Ichnaea_Com PRIVATE Ichnaea_Headers Ichnaea_PicoHeaders)