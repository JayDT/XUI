# requires: cache/environment variable FBX_HOME or FBX_ROOT
# output:
#   FBX_FOUND
#   FBX_INCLUDE_DIRS
#   FBX_LIBRARIES
# warning: only support MSVC currently

include(FindPkgMacros)
include(acmake_collect_home)

findpkg_begin(FBX)

# only support MSVC currently
if(MSVC)
    acmake_collect_home(FBX)
    if(FBX_HOME)
        set(FBX_PREFIX_PATH "${FBX_HOME}")
        create_search_paths(FBX)
        if(MSVC80)
            set(FBX_LIBRARY_NAMES fbxsdk_md2005)
        elseif(MSVC90)
            set(FBX_LIBRARY_NAMES fbxsdk_md2008)
        elseif(MSVC10)
            set(FBX_LIBRARY_NAMES fbxsdk_md2010)
        else()
            message(FATAL_ERROR "Only support MSVC now!")
        endif()
        # only continue when lib name setted
        if(FBX_LIBRARY_NAMES)
            set(FBX_INCLUDE_DIR "${FBX_HOME}/include")
            get_debug_names(FBX_LIBRARY_NAMES)
            find_library(
                FBX_LIBRARY_REL
                NAMES ${FBX_LIBRARY_NAMES}
                HINTS ${FBX_LIB_SEARCH_PATH}
                )
            find_library(
                FBX_LIBRARY_DBG
                NAMES ${FBX_LIBRARY_NAMES_DBG}
                HINTS ${FBX_LIB_SEARCH_PATH}
                )
            make_library_set(FBX_LIBRARY)
        endif()
    endif()

    #set(FBX_DEFINITIONS -DKFBX_DLLINFO)
endif()

findpkg_finish(FBX)
