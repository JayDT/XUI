include(ans.qt_support)

# include and link with qt
# 
# usage:
#   acmake_qt_support(output_files output_libs [HEADER header1, header2 ...]
#       [UI ui1 ui2 ...] [RESOURCE resource1 resource2])
#   output_files and output_libs can be used in the following code
macro(acmake_qt_support)
    qt_support(${ARGN})
endmacro()
