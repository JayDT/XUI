# warning: do not use this directly, use acmake_xxx_support instead

include(acmake_common)

# usage: acmake_simple_support(<target> <library>)
macro(acmake_simple_support TARGET LIBRARY)
    find_package(${LIBRARY} REQUIRED)
    string(TOUPPER ${LIBRARY} UPPER_LIBRARY)
    include_directories(${${UPPER_LIBRARY}_INCLUDE_DIRS})
    target_link_libraries(${TARGET} ${${UPPER_LIBRARY}_LIBRARIES})
endmacro()
