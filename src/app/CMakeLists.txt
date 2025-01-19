file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB PROTO ${CMAKE_CURRENT_SOURCE_DIR}/proto/*.c)
file(GLOB PDI ${CMAKE_CURRENT_SOURCE_DIR}/pdi/*.cpp)
add_library(Ichnaea_Application STATIC ${SOURCES} ${PROTO} ${PDI})
target_link_libraries(Ichnaea_Application PRIVATE
  Ichnaea_Headers
  Ichnaea_PicoHeaders
)