# Integration of the mbedutils library
file(GLOB MBED_INT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(Ichnaea_MbedUtils STATIC ${MBED_INT_SOURCES})
target_link_libraries(Ichnaea_MbedUtils
  mbedutils_headers
  Ichnaea_Headers
  Ichnaea_PicoHeaders
)