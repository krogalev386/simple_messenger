if(BUILD_GOOGLE_TESTS)
    add_subdirectory(ut)
endif()

add_library(utilities_lib_if INTERFACE)
target_include_directories(utilities_lib_if
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(utilities_lib_if
    INTERFACE
        logger_lib_if
)
