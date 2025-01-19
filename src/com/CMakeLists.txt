file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB RPC_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/rpc/*.cpp)

add_library(Ichnaea_Com STATIC
  ${SOURCES}
  ${RPC_SOURCES}
)
target_link_libraries(Ichnaea_Com PRIVATE Ichnaea_Headers Ichnaea_PicoHeaders)