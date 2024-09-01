add_library(common_endpoints_lib EndpointBase.cpp)

target_include_directories(common_endpoints_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(common_endpoints_lib
    PUBLIC
        msg_itf_lib_if
        logger_lib_if
        comm_utils_lib
)