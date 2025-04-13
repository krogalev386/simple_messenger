add_library(msg_itf_lib_if INTERFACE)
target_include_directories(msg_itf_lib_if
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(protobuf_itf_lib
    grpc_services.proto
    proto_defines.proto
)

protobuf_generate(
    LANGUAGE cpp
    TARGET protobuf_itf_lib
)

protobuf_generate(
    LANGUAGE grpc
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
    TARGET protobuf_itf_lib
    PLUGIN "protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin"
)

target_link_libraries(protobuf_itf_lib
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc
        gRPC::grpc++
)

target_include_directories(protobuf_itf_lib
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(protobuf_itf_lib PROPERTIES CXX_CLANG_TIDY "")
