macro(list_sub_directories result curdir filter)
    file(GLOB children relative ${curdir}/${filter})
    set(dirlist "")

    foreach(child ${children})
        if(IS_DIRECTORY ${child})
            set(dirlist ${dirlist} ${child})
        endif()
    endforeach()

    set(${result} ${dirlist})
endmacro()