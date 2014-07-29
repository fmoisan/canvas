
file(GLOB sources *.cpp)
file(GLOB headers *.hpp)

source_group(src FILES ${sources} ${headers})

add_library(canvas ${sources} ${headers})

set_target_properties(canvas PROPERTIES FOLDER "libs")
