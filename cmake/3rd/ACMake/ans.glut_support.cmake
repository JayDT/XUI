include(ans.env)
include(ans.parse_arguments)

# gl_support([<target_name>])
#
# Original FindGLUT.cmake cannot find it in Microsoft SDK path.
# @praram target_name live empty if target is not executable
macro(glut_support)
    parse_arguments(
        GLUT_SUPPORT
        ""
        ""
        ${ARGN}
        )
    car(GLUT_SUPPORT_TARGET ${GLUT_SUPPORT_DEFAULT_ARGS})
    find_package(GLUT)
    if(NOT GLUT_FOUND)
        # see <http://public.kitware.com/Bug/view.php?id=853>
        set(GLUT_glut_LIBRARY glut32)
        set(GLUT_LIBRARIES ${GLUT_glut_LIBRARY})
        set(GLUT_FOUND TRUE)
    endif()
endmacro()
