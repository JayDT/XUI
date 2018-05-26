include(ans.env)
include(ans.parse_arguments)
include(ans.glew_support)
include(ans.glut_support)

# gl_support([<target_name>] [GLEW] [GLUT])
# @praram target_name live empty if target is not executable
macro(gl_support)
    parse_arguments(
        GL_SUPPORT
        ""
        "GLEW;GLUT"
        ${ARGN}
        )
    car(GL_SUPPORT_TARGET ${GL_SUPPORT_DEFAULT_ARGS})
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    if(GL_SUPPORT_TARGET)
        target_link_libraries(${GL_SUPPORT_TARGET} ${OPENGL_LIBRARIES})
    endif()
    if(GL_SUPPORT_GLEW)
        glew_support(${GL_SUPPORT_TARGET})
    endif()
    if(GL_SUPPORT_GLUT)
        glut_support(${GL_SUPPORT_TARGET})
    endif()
endmacro()
