# warning: use `acmake_group_source` instead, this file may be removed in the
#   future

include(ans.parse_arguments)

macro(source_group_by_dir source_files)
    if(MSVC)
        set(sgbd_cur_dir ${CMAKE_CURRENT_SOURCE_DIR})
        foreach(sgbd_file ${${source_files}})
            string(REGEX REPLACE ${sgbd_cur_dir}/\(.*\) \\1 sgbd_fpath ${sgbd_file})
            string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
            string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
            string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            if(sgbd_nogroup)
                set(sgbd_group_name "\\")
            endif(sgbd_nogroup)
            source_group(${sgbd_group_name} FILES ${sgbd_file})
        endforeach(sgbd_file)
    endif(MSVC)
endmacro(source_group_by_dir)

# new version
# usage:
#   group_source([RELATIVE path] source1 source2 ...)
macro(group_source)
    PARSE_ARGUMENTS(GROUP_SOURCE
        "RELATIVE"
        ""
        ${ARGN}
        )
    if(GROUP_SOURCE_RELATIVE)
        set(BASE_DIR ${GROUP_SOURCE_RELATIVE})
        set(source_files ${GROUP_SOURCE_DEFAULT_ARGS})
        foreach(sgbd_file ${source_files})
            file(RELATIVE_PATH sgbd_fpath "${BASE_DIR}" "${sgbd_file}")
            string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
            string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
            string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            if(sgbd_nogroup)
                set(sgbd_group_name "\\")
            endif(sgbd_nogroup)
            source_group(${sgbd_group_name} FILES ${sgbd_file})
        endforeach(sgbd_file)
    else()
        source_group_by_dir(GROUP_SOURCE_DEFAULT_ARGS)
    endif()
endmacro()
