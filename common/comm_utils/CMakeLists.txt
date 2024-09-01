add_library(comm_utils_lib SocketPolling.cpp)
target_include_directories(comm_utils_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(comm_utils_lib
    PRIVATE
        logger_lib_if
)