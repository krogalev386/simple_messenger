add_subdirectory(endpoint)

add_executable(client_app client_app_main.cpp)

target_link_libraries(client_app
    PRIVATE
        msg_itf_lib_if
        client_endpoints_lib
)