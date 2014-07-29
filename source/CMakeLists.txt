cmake_minimum_required(VERSION 2.8)

project(canvas)

include(cmake/config.cmake)

add_subdirectory(external)
add_subdirectory(libs)
add_subdirectory(test)
