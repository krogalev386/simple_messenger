add_subdirectory(authentification_service)
add_subdirectory(endpoint)
add_subdirectory(psql_manager)
add_subdirectory(server_manager)

add_executable(server_app server_app_main.cpp)

target_link_libraries(server_app
    PRIVATE
        msg_itf_lib_if
        server_manager_lib
        server_endpoints_lib
)