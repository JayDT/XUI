# Find OpenMesh.
# 
# OpenMesh_FOUND
# OpenMesh_INCLUDE_DIRS
# OpenMesh_CORE_LIBRARIES
# OpenMesh_TOOLS_LIBRARIES
#
# Environment variable OpenMesh_HOME mest be defined.
# Or there is a submodule localed in "misc/3rd/OpenMesh".
#
# @author Answeror
# @date 2012-03-25

include(FindPkgMacros)

findpkg_begin(OpenMesh)

# get home path from enviounment
getenv_path(OpenMesh_HOME)
if(NOT ENV_OpenMesh_HOME)
    getenv_path(OpenMesh_ROOT)
    set(ENV_OpenMesh_HOME ${ENV_OpenMesh_ROOT})
endif()

# from source directory
if(NOT ENV_OpenMesh_HOME)
    if(EXISTS ${CMAKE_SOURCE_DIR}/misc/3rd/OpenMesh)
        set(ENV_OpenMesh_HOME ${CMAKE_SOURCE_DIR}/misc/3rd/OpenMesh)
    endif()
endif()

if(ENV_OpenMesh_HOME)
    set(OpenMesh_PREFIX_PATH ${ENV_OpenMesh_HOME})
    create_search_paths(OpenMesh)
    set(OpenMesh_INCLUDE_DIR ${ENV_OpenMesh_HOME}/include)

    set(OpenMesh_CORE_LIBRARY_NAMES OpenMeshCore)
    get_debug_names(OpenMesh_CORE_LIBRARY_NAMES)
    find_library(OpenMesh_CORE_LIBRARY_REL NAMES ${OpenMesh_CORE_LIBRARY_NAMES} HINTS ${OpenMesh_LIB_SEARCH_PATH} PATH_SUFFIXES "" vc)
    find_library(OpenMesh_CORE_LIBRARY_DBG NAMES ${OpenMesh_CORE_LIBRARY_NAMES_DBG} HINTS ${OpenMesh_LIB_SEARCH_PATH} PATH_SUFFIXES "" vc)
    make_library_set(OpenMesh_CORE_LIBRARY)

    set(OpenMesh_TOOLS_LIBRARY_NAMES OpenMeshTools)
    get_debug_names(OpenMesh_TOOLS_LIBRARY_NAMES)
    find_library(OpenMesh_TOOLS_LIBRARY_REL NAMES ${OpenMesh_TOOLS_LIBRARY_NAMES} HINTS ${OpenMesh_LIB_SEARCH_PATH} PATH_SUFFIXES "" vc)
    find_library(OpenMesh_TOOLS_LIBRARY_DBG NAMES ${OpenMesh_TOOLS_LIBRARY_NAMES_DBG} HINTS ${OpenMesh_LIB_SEARCH_PATH} PATH_SUFFIXES "" vc)
    make_library_set(OpenMesh_TOOLS_LIBRARY)

    foreach(SUFFIX "" _REL _DBG)
        set(
            OpenMesh_LIBRARY${SUFFIX}
            ${OpenMesh_CORE_LIBRARY}${SUFFIX}
            ${OpenMesh_TOOLS_LIBRARY}${SUFFIX}
            )
    endforeach()
endif()

findpkg_finish(OpenMesh)
