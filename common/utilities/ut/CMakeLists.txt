add_executable(ut_circular_buffer TestCircularBuffer.cpp)
target_include_directories(ut_circular_buffer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(ut_circular_buffer
    PRIVATE
        utilities_lib_if
        gtest_main
)
add_test(NAME test_circular_buffer COMMAND ut_circular_buffer)
