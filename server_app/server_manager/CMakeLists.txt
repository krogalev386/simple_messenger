add_library(server_manager_lib ServerManager.cpp)
target_include_directories(server_manager_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(server_manager_lib
    PUBLIC
        server_endpoints_lib
        logger_lib_if
        thread_manager_lib
        utilities_lib_if
)