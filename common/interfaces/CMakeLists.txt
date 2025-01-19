add_library(msg_itf_lib_if INTERFACE)
target_include_directories(msg_itf_lib_if
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

protobuf_generate_cpp(
    PROTO_SRCS
    PROTO_HDRS
    proto_defines.proto
)

add_library(protobuf_itf_lib
    ${PROTO_SRCS}
)

target_link_libraries(protobuf_itf_lib
    PUBLIC
        ${Protobuf_LIBRARIES}
)

message(STATUS ${Protobuf_VERSION})

if (Protobuf_VERSION VERSION_GREATER_EQUAL 4)
    find_package(absl REQUIRED)
    target_link_libraries(protobuf_itf_lib
        PUBLIC
            absl::log_internal_check_op
    )
endif()

target_include_directories(protobuf_itf_lib
    PUBLIC
        ${Protobuf_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
)

set_target_properties(protobuf_itf_lib PROPERTIES CXX_CLANG_TIDY "")
