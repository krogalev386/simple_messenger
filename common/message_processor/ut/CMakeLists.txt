add_executable(ut_message_processing TestMessageProcessing.cpp)
target_include_directories(ut_message_processing
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(ut_message_processing
    PRIVATE
        message_processor_lib_if
        gtest_main
)
add_test(NAME test_message_processing COMMAND ut_message_processing)
