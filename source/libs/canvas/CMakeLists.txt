
file(GLOB_RECURSE sources *.cpp)
file(GLOB_RECURSE headers *.h *.hpp)

source_group(src FILES ${sources} ${headers})

find_package (Threads)
add_library(canvas ${sources} ${headers})
target_link_libraries(canvas ${CMAKE_THREAD_LIBS_INIT})

set_target_properties(canvas PROPERTIES FOLDER "libs")
