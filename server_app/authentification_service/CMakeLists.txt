add_library(auth_service_lib AuthentificationService.cpp)
target_include_directories(auth_service_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(auth_service_lib
    PUBLIC
        msg_itf_lib_if
        message_processor_lib_if
        logger_lib_if
        utilities_lib_if
        psql_manager_lib
)