file(GLOB sources *.cpp)
file(GLOB headers *.hpp)

source_group(src FILES ${sources} ${headers})

add_executable(test_canvas ${sources} ${headers} ${catch_headers})
target_link_libraries(test_canvas canvas)

add_custom_command(TARGET test_canvas POST_BUILD COMMAND test_canvas)

set_target_properties(test_canvas PROPERTIES FOLDER "tests")

add_test(canvas test_canvas)
