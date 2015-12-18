set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/compiler ${CMAKE_MODULE_PATH})

cmake_policy(SET CMP0054 NEW)

# compiler settings
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    include(clang)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    include(gcc)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    include(msvc)
endif()

# default to debug builds
if(CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(config cmake/config.cmake)

set(compilers cmake/compiler/gcc.cmake
              cmake/compiler/msvc.cmake)

set(modules cmake/modules/list_sub_directories.cmake)

source_group(cmake FILES ${config})
source_group(cmake\\compiler FILES ${compilers})
source_group(cmake\\modules FILES ${modules})

add_custom_target(canvas_config SOURCES ${config} ${compilers} ${modules})
