cmake_minimum_required(VERSION 3.11)

project(net_app)

cmake_policy(SET CMP0076 NEW)

add_subdirectory(common)
add_subdirectory(client_app)
add_subdirectory(server_app)
