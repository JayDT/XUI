include(acmake_common)
include(acmake_parse_arguments)

# usage:
#   cmake_add_imported_library(
#       <name>
#       [STATIC_DEBUG <path>]
#       [STATIC_RELEASE <path>]
#       [SHARED_DEBUG <path>]
#       [SHARED_RELEASE <path>]
# output:
#   <name>_STATIC
#   <name>_SHARED
macro(acmake_add_imported_library)
    PARSE_ARGUMENTS(
        ACMAKE_ADD_IMPORTED_LIBRARY
        "STATIC_DEBUG;STATIC_RELEASE;SHARED_DEBUG;SHARED_RELEASE"
        ""
        ${ARGN}
        )
    set(NAME ${ACMAKE_ADD_IMPORTED_LIBRARY_DEFAULT_ARGS})
    if(ACMAKE_ADD_IMPORTED_LIBRARY_STATIC_DEBUG OR
            ACMAKE_ADD_IMPORTED_LIBRARY_STATIC_RELEASE)
        message("asdf ${NAME}_STATIC")
        add_library(${NAME}_STATIC STATIC IMPORTED)
        if(ACMAKE_ADD_IMPORTED_LIBRARY_STATIC_DEBUG)
            set_property(TARGET ${NAME}_STATIC PROPERTY
                IMPORTED_LOCATION_DEBUG
                "${ACMAKE_ADD_IMPORTED_LIBRARY_STATIC_DEBUG}")
        endif()
        if(ACMAKE_ADD_IMPORTED_LIBRARY_STATIC_RELEASE)
            set_property(TARGET ${NAME}_STATIC PROPERTY
                IMPORTED_LOCATION_RELEASE
                "${ACMAKE_ADD_IMPORTED_LIBRARY_STATIC_RELEASE}")
        endif()
    endif()
    if(ACMAKE_ADD_IMPORTED_LIBRARY_SHARED_DEBUG OR
            ACMAKE_ADD_IMPORTED_LIBRARY_SHARED_RELEASE)
        add_library(${NAME}_SHARED SHARED IMPORTED)
        if(ACMAKE_ADD_IMPORTED_LIBRARY_SHARED_DEBUG)
            set_property(TARGET ${NAME}_SHARED PROPERTY
                IMPORTED_LOCATION_DEBUG
                "${ACMAKE_ADD_IMPORTED_LIBRARY_SHARED_DEBUG}")
        endif()
        if(ACMAKE_ADD_IMPORTED_LIBRARY_SHARED_RELEASE)
            set_property(TARGET ${NAME}_SHARED PROPERTY
                IMPORTED_LOCATION_RELEASE
                "${ACMAKE_ADD_IMPORTED_LIBRARY_SHARED_RELEASE}")
        endif()
    endif()
endmacro()
