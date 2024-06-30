add_library(server_endpoints_lib ServerEndpoint.cpp)
target_include_directories(server_endpoints_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(server_endpoints_lib
    PUBLIC
        logger_lib_if
        common_endpoints_lib
)