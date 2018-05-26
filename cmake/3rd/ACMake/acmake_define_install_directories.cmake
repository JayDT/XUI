include(acmake_common)
include(acmake_parse_arguments)

# usage: acmake_define_install_directories([NAME <name>])
# output:
#   [<name>_]INSTALL_ARCHIVE_DIR 
#   [<name>_]INSTALL_RUNTIME_DIR 
#   [<name>_]INSTALL_INCLUDE_DIR 
#   [<name>_]INSTALL_CMAKE_DIR 
#   [<name>_]INCLUDE_RELATIVE_TO_CMAKE 
#   [<name>_]ARCHIVE_RELATIVE_TO_CMAKE 
# see <http://www.vtk.org/Wiki/CMake/Tutorials/How_to_create_a_ProjectConfig.cmake_file>
# for details
macro(acmake_define_install_directories)
    PARSE_ARGUMENTS(
        ACMAKE_DEFINE_INSTALL_DIRECTORIES
        "NAME"
        ""
        ${ARGN}
        )
    if(ACMAKE_DEFINE_INSTALL_DIRECTORIES_NAME)
        set(ACMAKE_DEFINE_INSTALL_DIRECTORIES_PREFIX
            ${ACMAKE_DEFINE_INSTALL_DIRECTORIES_NAME}_)
    else()
        set(ACMAKE_DEFINE_INSTALL_DIRECTORIES_PREFIX "")
    endif()

    set(${ACMAKE_DEFINE_INSTALL_DIRECTORIES_PREFIX}INSTALL_ARCHIVE_DIR
        lib CACHE PATH "Installation directory for libraries")
    set(${ACMAKE_DEFINE_INSTALL_DIRECTORIES_PREFIX}INSTALL_RUNTIME_DIR
        bin CACHE PATH "Installation directory for excutables")
    set(${ACMAKE_DEFINE_INSTALL_DIRECTORIES_PREFIX}INSTALL_INCLUDE_DIR
        include CACHE PATH "Installation directory for header files")
    set(${ACMAKE_DEFINE_INSTALL_DIRECTORIES_PREFIX}INSTALL_CMAKE_DIR
        cmake CACHE PATH "Installation directory for CMake files")

    # make relative path absolute
    foreach(p ARCHIVE RUNTIME INCLUDE CMAKE)
        set(var ${ACMAKE_DEFINE_INSTALL_DIRECTORIES_PREFIX}INSTALL_${p}_DIR)
        if(NOT IS_ABSOLUTE "${${var}}")
            set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
        endif()
    endforeach()

    file(RELATIVE_PATH
        ${ACMAKE_DEFINE_INSTALL_DIRECTORIES_PREFIX}INCLUDE_RELATIVE_TO_CMAKE
        "${${ACMAKE_DEFINE_INSTALL_DIRECTORIES_PREFIX}INSTALL_CMAKE_DIR}"
        "${${ACMAKE_DEFINE_INSTALL_DIRECTORIES_PREFIX}INSTALL_INCLUDE_DIR}")
    file(RELATIVE_PATH
        ${ACMAKE_DEFINE_INSTALL_DIRECTORIES_PREFIX}ARCHIVE_RELATIVE_TO_CMAKE
        "${${ACMAKE_DEFINE_INSTALL_DIRECTORIES_PREFIX}INSTALL_CMAKE_DIR}"
        "${${ACMAKE_DEFINE_INSTALL_DIRECTORIES_PREFIX}INSTALL_ARCHIVE_DIR}")
endmacro()
