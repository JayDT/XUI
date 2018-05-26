include(acmake_common)

# Usage:
#   acmake_set_output_dir(${CMAKE_BINARY_DIR})
macro(acmake_set_output_dir DIR)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DIR})
endmacro()
