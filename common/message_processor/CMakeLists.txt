add_library(message_processor_lib_if INTERFACE)
target_include_directories(message_processor_lib_if
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(message_processor_lib_if
    INTERFACE
        msg_itf_lib_if
)