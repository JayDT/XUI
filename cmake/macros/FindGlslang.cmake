# - Try to find Glslanglib
# Once done, this will define
#
#  Glslang_FOUND - system has Glslang
#  Glslang_INCLUDE_DIRS - the Glslang include directories 
#  Glslang_LIBRARIES - link these to use Glslang

include(FindPkgMacros)
findpkg_begin(Glslang)

# construct search paths
set(Glslang_PREFIX_PATH ${Glslang_HOME} ${ENV_Glslang_HOME})
create_search_paths(Glslang)

find_path(Glslang_INCLUDE_DIR NAMES glslang/Public/ShaderLang.h HINTS ${Glslang_INC_SEARCH_PATH} ${CMAKE_FIND_ROOT_PATH})
find_path(SPIRV_INCLUDE_DIR NAMES SPIRV/spirv.hpp HINTS ${SPIRV_INC_SEARCH_PATH} ${CMAKE_FIND_ROOT_PATH})
#find_path(vkhlsl_INCLUDE_DIR NAMES hlsl/hlslOpMap.h HINTS ${vkhlsl_INC_SEARCH_PATH} ${CMAKE_FIND_ROOT_PATH})

set(Glslang_LIBRARY_NAMES glslang libglslang)
get_debug_names(Glslang_LIBRARY_NAMES)

find_library(Glslang_LIBRARY_REL NAMES ${Glslang_LIBRARY_NAMES} HINTS ${Glslang_LIB_SEARCH_PATH} ${CMAKE_FIND_ROOT_PATH}) # PATH_SUFFIXES "" glslang/Release glslang/RelWithDebInfo glslang/MinSizeRel)
find_library(Glslang_LIBRARY_DBG NAMES ${Glslang_LIBRARY_NAMES_DBG} HINTS ${Glslang_LIB_SEARCH_PATH} ${CMAKE_FIND_ROOT_PATH}) # PATH_SUFFIXES "" glslang/Debug)

set(SPIRV_LIBRARY_NAMES SPIRV libSPIRV)
get_debug_names(SPIRV_LIBRARY_NAMES)

find_library(SPIRV_LIBRARY_REL NAMES ${SPIRV_LIBRARY_NAMES} HINTS ${SPIRV_LIB_SEARCH_PATH} ${CMAKE_FIND_ROOT_PATH}) # PATH_SUFFIXES "" SPIRV/Release SPIRV/RelWithDebInfo SPIRV/MinSizeRel)
find_library(SPIRV_LIBRARY_DBG NAMES ${SPIRV_LIBRARY_NAMES_DBG} HINTS ${SPIRV_LIB_SEARCH_PATH} ${CMAKE_FIND_ROOT_PATH}) # PATH_SUFFIXES "" SPIRV/Debug)

set(SPIRV_REMAPPER_LIBRARY_NAMES SPVRemapper libSPVRemapper)
get_debug_names(SPIRV_REMAPPER_LIBRARY_NAMES)

find_library(SPIRV_REMAPPER_LIBRARY_REL NAMES ${SPIRV_REMAPPER_LIBRARY_NAMES} HINTS ${SPIRV_LIB_SEARCH_PATH} ${CMAKE_FIND_ROOT_PATH}) # PATH_SUFFIXES "" SPIRV/Release SPIRV/RelWithDebInfo SPIRV/MinSizeRel)
find_library(SPIRV_REMAPPER_LIBRARY_DBG NAMES ${SPIRV_REMAPPER_LIBRARY_NAMES_DBG} HINTS ${SPIRV_LIB_SEARCH_PATH} ${CMAKE_FIND_ROOT_PATH}) # PATH_SUFFIXES "" SPIRV/Debug)

set(vkhlsl_LIBRARY_NAMES HLSL libHLSL)
get_debug_names(vkhlsl_LIBRARY_NAMES)

find_library(vkhlsl_LIBRARY_REL NAMES ${vkhlsl_LIBRARY_NAMES} HINTS ${vkhlsl_LIB_SEARCH_PATH} ${CMAKE_FIND_ROOT_PATH}) # PATH_SUFFIXES "" hlsl/Release hlsl/RelWithDebInfo hlsl/MinSizeRel)
find_library(vkhlsl_LIBRARY_DBG NAMES ${vkhlsl_LIBRARY_NAMES_DBG} HINTS ${vkhlsl_LIB_SEARCH_PATH} ${CMAKE_FIND_ROOT_PATH}) # PATH_SUFFIXES "" hlsl/Debug)

set(OSDEP_LIBRARY_NAMES OSDependent libOSDependent)
get_debug_names(OSDEP_LIBRARY_NAMES)

#if(WIN32)
#    set(OSDEP_LIB_SEARCH_PATH_SUFFIX "OSDependent/Windows")
#elseif(UNIX)
#    set(OSDEP_LIB_SEARCH_PATH_SUFFIX "OSDependent/Unix")
#else(WIN32)
#    message("unknown platform")
#endif(WIN32)

find_library(OSDEP_LIBRARY_REL NAMES ${OSDEP_LIBRARY_NAMES} HINTS ${OSDEP_LIB_SEARCH_PATH} ${CMAKE_FIND_ROOT_PATH}) # PATH_SUFFIXES "" ${OSDEP_LIB_SEARCH_PATH_SUFFIX}/Release ${OSDEP_LIB_SEARCH_PATH_SUFFIX}/RelWithDebInfo ${OSDEP_LIB_SEARCH_PATH_SUFFIX}/MinSizeRel)
find_library(OSDEP_LIBRARY_DBG NAMES ${OSDEP_LIBRARY_NAMES_DBG} HINTS ${OSDEP_LIB_SEARCH_PATH} ${CMAKE_FIND_ROOT_PATH}) # PATH_SUFFIXES "" ${OSDEP_LIB_SEARCH_PATH_SUFFIX}/Debug)

make_library_set(Glslang_LIBRARY)
make_library_set(SPIRV_LIBRARY)
make_library_set(SPIRV_REMAPPER_LIBRARY)
make_library_set(vkhlsl_LIBRARY)
make_library_set(OSDEP_LIBRARY)

set(Glslang_INCLUDE_DIR ${Glslang_INCLUDE_DIR} ${SPIRV_INCLUDE_DIR}) # ${vkhlsl_INCLUDE_DIR})
set(Glslang_LIBRARY ${Glslang_LIBRARY} ${SPIRV_LIBRARY} ${SPIRV_REMAPPER_LIBRARY} ${vkhlsl_LIBRARY} ${OSDEP_LIBRARY})

findpkg_finish(Glslang)

