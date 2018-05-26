include(ans.parse_arguments)

# see <http://www.itk.org/Wiki/CMakeMacroParseArguments>
macro(acmake_parse_arguments prefix arg_names option_names)
    parse_arguments(
        "${prefix}"
        "${arg_names}"
        "${option_names}"
        ${ARGN}
        )
endmacro()
