# PUREMVC_FOUND
# PUREMVC_INCLUDE_DIRS
# PUREMVC_LIBRARIES
# answeror at 2011-10-30

if(NOT PUREMVC_FOUND)
    include(FindPkgMacros)
    findpkg_begin(PUREMVC)

    # get home path from enviounment
    getenv_path(PUREMVC_HOME)
    if(NOT ENV_PUREMVC_HOME)
        getenv_path(PUREMVC_ROOT)
        set(ENV_PUREMVC_HOME ${ENV_PUREMVC_ROOT})
    endif()


    if(ENV_PUREMVC_HOME)
        set(PUREMVC_PREFIX_PATH ${ENV_PUREMVC_HOME})
        create_search_paths(PUREMVC)
        set(PUREMVC_INCLUDE_DIR ${ENV_PUREMVC_HOME}/include)
        set(PUREMVC_LIBRARY_NAMES PureMVC)
        set(PUREMVC_LIBRARY_NAMES_DBG PureMVCd)
        #get_debug_names(PUREMVC_LIBRARY_NAMES)
        find_library(PUREMVC_LIBRARY_REL NAMES ${PUREMVC_LIBRARY_NAMES} HINTS ${PUREMVC_LIB_SEARCH_PATH} PATH_SUFFIXES "" vc)
        find_library(PUREMVC_LIBRARY_DBG NAMES ${PUREMVC_LIBRARY_NAMES_DBG} HINTS ${PUREMVC_LIB_SEARCH_PATH} PATH_SUFFIXES "" vc)
        make_library_set(PUREMVC_LIBRARY)
    endif()

    findpkg_finish(PUREMVC)
endif()
