include(acmake_common)
include(acmake_env_home)
#include(acmake_parse_arguments)

# usage: acmake_bullet_support([<target>])
#   if target is not specified, no linking
# requires: environment variable BULLET_HOME or BULLET_ROOT
macro(acmake_bullet_support)
    acmake_env_home(BULLET_ROOT BULLET)
    find_package(Bullet REQUIRED)
    include_directories(${BULLET_INCLUDE_DIRS})
    if(${ARGC} GREATER 0)
        acmake_bullet_support_link(${ARGN})
    endif()
endmacro()

function(acmake_bullet_support_link TARGET)
    target_link_libraries(${TARGET} ${BULLET_LIBRARIES})
endfunction()
