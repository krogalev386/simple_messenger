add_library(msg_itf_lib_if INTERFACE)
target_include_directories(msg_itf_lib_if
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(protobuf_itf_lib)

protobuf_generate(
    LANGUAGE cpp
    TARGET protobuf_itf_lib
    PROTOS proto_defines.proto
)

target_link_libraries(protobuf_itf_lib
    PUBLIC
        protobuf::libprotobuf
        protobuf::libprotoc
)

target_include_directories(protobuf_itf_lib
    PUBLIC
        ${Protobuf_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(protobuf_itf_lib PROPERTIES CXX_CLANG_TIDY "")
