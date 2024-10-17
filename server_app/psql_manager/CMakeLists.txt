add_library(psql_manager_lib PsqlManager.cpp)
target_include_directories(psql_manager_lib
    PUBLIC
        /usr/include/postgresql
        ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(psql_manager_lib
    PRIVATE
        pq
    PUBLIC
        msg_itf_lib_if
        logger_lib_if
        utilities_lib_if
)
