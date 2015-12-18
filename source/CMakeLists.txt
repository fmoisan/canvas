cmake_minimum_required(VERSION 2.8)

project(canvas)

set(BUILD_SHARED_LIBS FALSE)

# add external first, use their proper settings
add_subdirectory(external)

include(cmake/config.cmake)
include(CTest)

add_subdirectory(libs)
add_subdirectory(test)
