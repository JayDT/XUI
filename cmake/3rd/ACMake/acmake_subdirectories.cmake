include(acmake_common)
include(list_subdirectories)
include(acmake_parse_arguments)

# usage: acmake_subdirectories(<output var> [RELATIVE])
macro(acmake_subdirectories)
    PARSE_ARGUMENTS(
        ACMAKE_SUBDIRECTORIES
        ""
        "RELATIVE"
        ${ARGN}
        )
    if(ACMAKE_SUBDIRECTORIES_RELATIVE)
        list_subdirectories(
            ${ACMAKE_SUBDIRECTORIES_DEFAULT_ARGS}
            ${CMAKE_CURRENT_SOURCE_DIR}
            TRUE
            )
    else()
        list_subdirectories(
            ${ACMAKE_SUBDIRECTORIES_DEFAULT_ARGS}
            ${CMAKE_CURRENT_SOURCE_DIR}
            FALSE
            )
    endif()
endmacro()
