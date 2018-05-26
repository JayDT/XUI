# ANS_FOUND
# ANS_INCLUDE_DIRS
# ANS_LIBRARIES
# answeror at 2011-11-05

include(FindPkgMacros)
findpkg_begin(ANS)

# get home path from enviounment
getenv_path(ANS_HOME)
if(NOT ENV_ANS_HOME)
    getenv_path(ANS_ROOT)
    set(ENV_ANS_HOME ${ENV_ANS_ROOT})
endif()

# from source directory
if(NOT ENV_ANS_HOME)
    if(EXISTS ${CMAKE_SOURCE_DIR}/misc/3rd/ans)
        set(ENV_ANS_HOME ${CMAKE_SOURCE_DIR}/misc/3rd/ans)
    endif()
endif()

if(ENV_ANS_HOME)
    set(ANS_PREFIX_PATH ${ENV_ANS_HOME})
    create_search_paths(ANS)
    set(ANS_INCLUDE_DIR ${ENV_ANS_HOME}/include)
    set(ANS_LIBRARY_NAMES ans)
    get_debug_names(ANS_LIBRARY_NAMES)
    find_library(ANS_LIBRARY_REL NAMES ${ANS_LIBRARY_NAMES} HINTS ${ANS_LIB_SEARCH_PATH} PATH_SUFFIXES "" vc)
    find_library(ANS_LIBRARY_DBG NAMES ${ANS_LIBRARY_NAMES_DBG} HINTS ${ANS_LIB_SEARCH_PATH} PATH_SUFFIXES "" vc)
    make_library_set(ANS_LIBRARY)
endif()

findpkg_finish(ANS)
