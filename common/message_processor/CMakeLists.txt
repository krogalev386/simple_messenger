add_library(message_processor_lib MessageProcessing.cpp)
target_include_directories(message_processor_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(message_processor_lib
    PUBLIC
        logger_lib_if
        msg_itf_lib_if
        common_endpoints_lib
)