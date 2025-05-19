if(BUILD_GOOGLE_TESTS)
    add_subdirectory(ut)
endif()

add_library(message_processor_lib_if INTERFACE)
target_include_directories(message_processor_lib_if
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(message_processor_lib_if
    INTERFACE
        logger_lib_if
        msg_itf_lib_if
        protobuf_itf_lib
)