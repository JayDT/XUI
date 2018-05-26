include(ans.source_group_by_dir)

# usage:
#   acmake_group_source([RELATIVE path] source1 source2 ...)
macro(acmake_group_source)
    group_source(${ARGN})
endmacro()
