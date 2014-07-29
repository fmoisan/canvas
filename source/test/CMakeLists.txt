
include(list_sub_directories)

list_sub_directories(test_directories "${CMAKE_CURRENT_SOURCE_DIR}" "*")

set(catch_source_dir "../external/catch/single_include")

include_directories(${catch_source_dir})
include_directories(../libs)

file(GLOB catch_headers ${catch_source_dir}/*.hpp)
source_group(src\\catch FILES ${catch_headers})

add_subdirectory(test_canvas)
