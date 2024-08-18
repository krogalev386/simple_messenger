add_library(logger_lib_if INTERFACE)
target_include_directories(logger_lib_if
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(logger_lib_if
    INTERFACE
        utilities_lib_if
)