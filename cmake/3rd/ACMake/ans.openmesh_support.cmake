include(ans.env)
include(ans.parse_arguments)

# openmesh_support([<target_name>])
# @praram target_name live empty if target is not executable
macro(openmesh_support)
    parse_arguments(
        OPENMESH_SUPPORT
        ""
        ""
        ${ARGN}
        )
    car(OPENMESH_SUPPORT_TARGET ${OPENMESH_SUPPORT_DEFAULT_ARGS})
    find_package(OpenMesh REQUIRED)
    include_directories(${OpenMesh_INCLUDE_DIRS})
    add_definitions(-D_USE_MATH_DEFINES)
    if(OPENMESH_SUPPORT_TARGET)
        target_link_libraries(
            "${OPENMESH_SUPPORT_TARGET}"
            ${OpenMesh_LIBRARIES}
            )
    endif()
endmacro()
