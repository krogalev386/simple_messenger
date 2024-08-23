add_library(thread_manager_lib ThreadManager.cpp)
target_include_directories(thread_manager_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(thread_manager_lib
    PUBLIC
        logger_lib_if
        utilities_lib_if
)