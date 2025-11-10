cmake_minimum_required(VERSION 3.15)
project(gMCP VERSION 1.0.0 LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required packages
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Generate gRPC and Protocol Buffer files
set(PROTO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/proto")
set(GENERATED_PROTOBUF_PATH "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${GENERATED_PROTOBUF_PATH})

set(GMCP_PROTO "${PROTO_PATH}/gmcp.proto")
set(GMCP_PROTO_SRCS "${GENERATED_PROTOBUF_PATH}/gmcp.pb.cc")
set(GMCP_PROTO_HDRS "${GENERATED_PROTOBUF_PATH}/gmcp.pb.h")
set(GMCP_GRPC_SRCS "${GENERATED_PROTOBUF_PATH}/gmcp.grpc.pb.cc")
set(GMCP_GRPC_HDRS "${GENERATED_PROTOBUF_PATH}/gmcp.grpc.pb.h")

# Custom command to generate protobuf and gRPC files
add_custom_command(
    OUTPUT "${GMCP_PROTO_SRCS}" "${GMCP_PROTO_HDRS}" "${GMCP_GRPC_SRCS}" "${GMCP_GRPC_HDRS}"
    COMMAND protobuf::protoc
    ARGS --grpc_out="${GENERATED_PROTOBUF_PATH}"
         --cpp_out="${GENERATED_PROTOBUF_PATH}"
         -I="${PROTO_PATH}"
         --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
         "${GMCP_PROTO}"
    DEPENDS "${GMCP_PROTO}"
    COMMENT "Generating gRPC and Protocol Buffer files"
)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${GENERATED_PROTOBUF_PATH}
)

# gMCP library (generated proto files)
add_library(gmcp_proto
    ${GMCP_PROTO_SRCS}
    ${GMCP_GRPC_SRCS}
)

target_link_libraries(gmcp_proto
    gRPC::grpc++
    protobuf::libprotobuf
)

target_include_directories(gmcp_proto PUBLIC
    ${GENERATED_PROTOBUF_PATH}
)

# gMCP Server executable
add_executable(gmcp_server
    src/server/main.cpp
    src/server/gmcp_server.cpp
    src/server/tool_manager.cpp
    src/server/memory_manager.cpp
)

target_link_libraries(gmcp_server
    gmcp_proto
    gRPC::grpc++
    gRPC::grpc++_reflection
    protobuf::libprotobuf
    Threads::Threads
)

# gMCP Client executable
add_executable(gmcp_client
    src/client/main.cpp
    src/client/gmcp_client.cpp
)

target_link_libraries(gmcp_client
    gmcp_proto
    gRPC::grpc++
    protobuf::libprotobuf
    Threads::Threads
)

# Installation rules
install(TARGETS gmcp_server gmcp_client
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${GENERATED_PROTOBUF_PATH}/
    DESTINATION include/gmcp
    FILES_MATCHING PATTERN "*.h"
)
