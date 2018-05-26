# warning: use acmake_subdirectories instead
macro(list_subdirectories retval curdir return_relative)

    file(GLOB sub-dir RELATIVE ${curdir} *)
    set(list_of_dirs "")
    foreach(dir ${sub-dir})
        if(IS_DIRECTORY ${curdir}/${dir})
            if(${return_relative})
                set(list_of_dirs ${list_of_dirs} ${dir})
            else()
                set(list_of_dirs ${list_of_dirs} ${curdir}/${dir})
            endif()
        endif()
    endforeach()
    set(${retval} ${list_of_dirs})
endmacro()

