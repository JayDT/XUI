# warning: use acmake_glew_support instead

include(ans.env)

# glew_support([<target_name>])
# @praram target_name live empty if target is not executable
macro(glew_support)
    find_package(GLEW REQUIRED)
    include_directories(${GLEW_INCLUDE_DIRS})
    if(${ARGC} GREATER 0)
        target_link_libraries(${ARGN} ${GLEW_LIBRARIES})
    endif()
endmacro()
