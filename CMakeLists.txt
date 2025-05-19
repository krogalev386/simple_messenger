cmake_minimum_required(VERSION 3.22)

project(net_app)

cmake_policy(SET CMP0076 NEW)

set(CMAKE_BUILD_TYPE Debug)
set(gRPC_PROTOBUF_PROVIDER package)
set(gRPC_PROTOBUF_PACKAGE_TYPE CONFIG)

if(CLANG_TIDY_CHECK)
    set(CMAKE_CXX_CLANG_TIDY clang-tidy -fix
        -checks=*,-llvmlibc-*,-modernize-*
    )
endif()

if(BUILD_GOOGLE_TESTS)
    enable_testing()
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG        52eb8108c5bdec04579160ae17225d66034bd723  #v1.17.0
    )
    FetchContent_MakeAvailable(googletest)
    add_custom_target(ut_target
        COMMENT "Target to build UT tests only"
    )
    add_dependencies(ut_target
        ut_circular_buffer
        ut_message_processing
    )
endif()

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
add_subdirectory(common)
add_subdirectory(client_app)
add_subdirectory(server_app)
