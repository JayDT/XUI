cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

project(XUI VERSION 0.0.1 LANGUAGES C CXX)

message("")
message("Configuration:")
message("")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
set( CMAKE_VERBOSE_MAKEFILE on )

set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${CMAKE_SOURCE_DIR}/cmake/macros
  ${CMAKE_SOURCE_DIR}/cmake/3rd/ACMake
  ${CMAKE_SOURCE_DIR}/cmake/extensions
)

include(cmake/defines.cmake)

# init ACMake
include(ans.common)
include(acmake_common)
include(acmake_mysql_support)

# set default buildoptions and print them
include(cmake/options.cmake)
include(Macros)
include(MetaParser)
include(ResourceCompiler)
include(CheckBuildDir)
include(CheckCXXSourceRuns)
include(CheckIncludeFiles)
include(CheckPlatform)
include(InstallRequiredSystemLibraries)
include(HlslUtils)
# Find revision ID and hash of the sourcetree
include(cmake/genrev.cmake)

if (NOT DEBUG AND INSTEX)
    include(CheckSSEFeatureSet)
    message(STATUS "Compiler Flags ${CMAKE_CXX_FLAGS}")
endif()

message("")
message("Information:")
message("------------------------------------------------------------------------------------------------------------------")
message("${PROJECT_NAME} revision : ${rev_hash}") # ${GIT_REVISION}
message("Core rev. date           : ${rev_date}")
message("Install prefix           : ${CMAKE_INSTALL_PREFIX}")
message("Install server to        : ${BIN_DIR}")
message("Install libaries to      : ${LIBS_DIR}")
message("Source Dir               : ${CMAKE_SOURCE_DIR}") 
message("------------------------------------------------------------------------------------------------------------------")
message("")
message("")
 
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH ${LIBS_DIR})
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include(ConfigureBoost)

message(STATUS "Find depency:")
find_package(PCHSupport)
find_package(LLVM REQUIRED)
find_package(CURL REQUIRED)

IF(UNIX)
    FIND_PACKAGE(Libdl REQUIRED)
    IF (NOT LIBDL_FOUND)
        MESSAGE(FATAL_ERROR "Dynamic Library handler missing: Can't load dynamic library.")
    ENDIF (NOT LIBDL_FOUND)

    # Look for binutils-dev. Needed for pretty stack printing.
    FIND_PACKAGE(BFD REQUIRED)
    IF (BFD_FOUND)
        MESSAGE(STATUS "Binutils found.")
        ADD_DEFINITIONS(-DHAVE_BFD)
        SET(HAVE_BFD 1)
    ELSE (BFD_FOUND)
        MESSAGE(FATAL_ERROR "Binutils-dev missing: No pretty stack-trace printing.")
    ENDIF (BFD_FOUND)
ENDIF(UNIX)

FIND_PACKAGE(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

# unix
if (UNIX)
    find_package(ZLIB REQUIRED)
    if (ZLIB_FOUND)
        mark_as_advanced( ZLIB_FOUND ZLIB_LIBRARIES ZLIB_INCLUDE_DIR ) 
        message( ${ASTATUS} "Found ZLib library        : ${ZLIB_LIBRARIES}")
        message( ${ASTATUS} "Found ZLib headers        : ${ZLIB_INCLUDE_DIR}")
        #workaround for ubuntu
        #SET(ZLIB_LIBRARIES "libz.so")
    else()
        MESSAGE(FATAL_ERROR "Zlib not found")
    endif(ZLIB_FOUND)
else(WIN32)
    SET(ZLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dep/zlib )
    SET(ZLIB_LIBRARIES zlib)
endif(UNIX)

#find_package(OpenGL) # ToDo
find_package(DirectX)
find_package(DirectX11)

message(STATUS "Complete")
message("")
message("")

# debug related
message(STATUS "CMAKE_C_FLAGS ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG}")

message("")
message("")

enable_testing()

message(STATUS "Prepare Source:")
ADD_SUBDIRECTORY(dep)
ADD_SUBDIRECTORY(src)
message(STATUS "Complete Source")
message("")
message("")
