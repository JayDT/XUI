include(acmake_common)
include(ans.glew_support)

# usage: acmake_glew_support([<target_name>])
#
# @praram target_name live empty if no linking required
macro(acmake_glew_support)
    glew_support(${ARGN})
endmacro()
