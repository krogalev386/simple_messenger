cmake_minimum_required(VERSION 3.11)

project(net_app)

cmake_policy(SET CMP0076 NEW)

set(CMAKE_BUILD_TYPE Debug)

if(CLANG_TIDY_CHECK)
    set(CMAKE_CXX_CLANG_TIDY clang-tidy -fix
        -checks=*,-llvmlibc-*,-modernize-*
    )
endif()

add_subdirectory(common)
add_subdirectory(client_app)
add_subdirectory(server_app)
