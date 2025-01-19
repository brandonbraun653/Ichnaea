# -----------------------------------------------------------------------------
# Help CMake find Protobuf and gRPC. This assumes that the Protobuf and gRPC
# have been installed system wide. See the Dockerfile for the installation
# steps.
#   - Installation: https://grpc.io/docs/languages/cpp/quickstart/
#   - Cmake examples: https://github.com/grpc/grpc/blob/v1.66.0/examples/cpp/cmake/common.cmake
# -----------------------------------------------------------------------------

# Find Protobuf installation
# Looks for protobuf-config.cmake file installed by Protobuf's cmake installation.
option(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
if(CMAKE_CROSSCOMPILING)
  find_program(_PROTOBUF_PROTOC protoc)
else()
  set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()


# Find gRPC installation
# Looks for gRPCConfig.cmake file installed by gRPC's cmake installation.
find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP gRPC::grpc++)
if(CMAKE_CROSSCOMPILING)
  find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
  set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()

# -----------------------------------------------------------------------------
# Add a target to generate the .pb.cc and .pb.h files
# -----------------------------------------------------------------------------
set(PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/protos)
file(GLOB PROTO_FILES ${PROTO_DIR}/*.proto)

# Directory for generated files
set(GENERATED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated)

# Custom command to generate all .proto files
add_custom_target(generate_grpc_sources ALL
    ${_PROTOBUF_PROTOC}
      --proto_path=${PROTO_DIR}
      --cpp_out=${GENERATED_DIR}
      --grpc_out=${GENERATED_DIR}
      --plugin=protoc-gen-grpc=${_GRPC_CPP_PLUGIN_EXECUTABLE}
      ${PROTO_FILES}
    DEPENDS ${PROTO_FILES}
)

# -----------------------------------------------------------------------------
# Add the simulator library
# -----------------------------------------------------------------------------
file(GLOB SIM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB SIM_HW_FAKE ${CMAKE_CURRENT_SOURCE_DIR}/hw/*.cpp)
file(GLOB GENERATED_STUBS ${GENERATED_DIR}/*.pb.cc)

add_library(Ichnaea_Simulator STATIC
  ${SIM_SOURCES}
  ${SIM_HW_FAKE}
  ${GENERATED_STUBS}
  ${MBEDUTILS_TEST_FAKE_DIR}/nor_flash_file.cpp
)

target_include_directories(Ichnaea_Simulator
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/hw
    ${MBEDUTILS_TEST_FAKE_DIR}
    ${GENERATED_DIR}
)

target_link_libraries(Ichnaea_Simulator
  PUBLIC
    absl::check
    absl::flags
    absl::flags_parse
    absl::log
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
  PRIVATE
    Ichnaea_Headers
    Ichnaea_PicoHeaders
    mbedutils_sim_headers
)
