include(acmake_common)

macro(acmake_pythonlib_support)
    parse_arguments(
        ACMAKE_PYTHONLIB_SUPPORT
        ""
        "LIB"
        ${ARGN}
        )
    # extract target name
    car(ACMAKE_PYTHONLIB_SUPPORT_TARGET ${ACMAKE_PYTHONLIB_SUPPORT_DEFAULT_ARGS})
    
endmacro()
