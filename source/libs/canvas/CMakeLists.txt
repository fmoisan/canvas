
file(GLOB_RECURSE sources *.cpp)
file(GLOB_RECURSE headers *.h *.hpp)

source_group(src FILES ${sources} ${headers})

add_library(canvas ${sources} ${headers})

set_target_properties(canvas PROPERTIES FOLDER "libs")
