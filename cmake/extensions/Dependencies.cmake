#######################################################################
# Merged From Ogre3D see http://www.ogre3d.org/
#######################################################################

include(MacroLogFeature)

#######################################################################
# Find all necessary and optional
#######################################################################

find_package(PCHSupport)
find_package(LLVM REQUIRED)
find_package(CURL REQUIRED)

#######################################################################
# Render Systems
#######################################################################

#find_package(OpenGL) # Use Build in
find_package(DirectX)
find_package(DirectX11)
find_package(Vulkan)

IF(UNIX)
    FIND_PACKAGE(Libdl REQUIRED)
    IF (NOT LIBDL_FOUND)
        MESSAGE(FATAL_ERROR "Dynamic Library handler missing: Can't load dynamic library.")
    ENDIF (NOT LIBDL_FOUND)

    #Required GCC  binutils-dev  
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

# PROJECT_DEPENDENCIES_DIR can be used to specify a single base
# folder where the required dependencies may be found.
set(PROJECT_DEPENDENCIES_DIR "" CACHE PATH "Path to prebuilt PROJECT dependencies")
option(PROJECT_BUILD_DEPENDENCIES "automatically build Ogre Dependencies (freetype, zzip, boost)" TRUE)
option(USE_INBUILD_GLSLANG "" ON)

include(FindPkgMacros)
getenv_path(PROJECT_DEPENDENCIES_DIR)
if(PROJECT_BUILD_PLATFORM_EMSCRIPTEN)
  set(PROJECT_DEP_SEARCH_PATH
    ${PROJECT_DEPENDENCIES_DIR}
    ${EMSCRIPTEN_ROOT_PATH}/system
    ${ENV_PROJECT_DEPENDENCIES_DIR}
    "${CMAKE_BINARY_DIR}/EmscriptenDependencies"
    "${CMAKE_SOURCE_DIR}/EmscriptenDependencies"
    "${CMAKE_BINARY_DIR}/../EmscriptenDependencies"
    "${CMAKE_SOURCE_DIR}/../EmscriptenDependencies"
  )
elseif(APPLE_IOS)
  set(PROJECT_DEP_SEARCH_PATH 
    ${PROJECT_DEPENDENCIES_DIR}
    ${ENV_PROJECT_DEPENDENCIES_DIR}
    "${CMAKE_BINARY_DIR}/iOSDependencies"
    "${CMAKE_SOURCE_DIR}/iOSDependencies"
    "${CMAKE_BINARY_DIR}/../iOSDependencies"
    "${CMAKE_SOURCE_DIR}/../iOSDependencies"
  )
elseif(PROJECT_BUILD_PLATFORM_ANDROID)
  set(PROJECT_DEP_SEARCH_PATH 
    ${PROJECT_DEPENDENCIES_DIR}
    ${ENV_PROJECT_DEPENDENCIES_DIR}
    "${CMAKE_BINARY_DIR}/AndroidDependencies"
    "${CMAKE_SOURCE_DIR}/AndroidDependencies"
    "${CMAKE_BINARY_DIR}/../AndroidDependencies"
    "${CMAKE_SOURCE_DIR}/../AndroidDependencies"
  )
else()
  set(PROJECT_DEP_SEARCH_PATH 
    ${PROJECT_DEPENDENCIES_DIR}
    ${ENV_PROJECT_DEPENDENCIES_DIR}
    "${CMAKE_BINARY_DIR}/Dependencies"
    "${CMAKE_SOURCE_DIR}/Dependencies"
    "${CMAKE_BINARY_DIR}/../Dependencies"
    "${CMAKE_SOURCE_DIR}/../Dependencies"
  )
endif()

message(STATUS "Search path: ${PROJECT_DEP_SEARCH_PATH}")
list(GET PROJECT_DEP_SEARCH_PATH 0 PROJECTDEPS_PATH)

if(CMAKE_CROSSCOMPILING)
    set(CMAKE_FIND_ROOT_PATH ${PROJECTDEPS_PATH} "${CMAKE_FIND_ROOT_PATH}")

    set(CROSS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE})
    
    if(ANDROID)
        set(CROSS ${CROSS}
            -DANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL}
            -DANDROID_ABI=${ANDROID_ABI}
            -DANDROID_NDK=${ANDROID_NDK})
    endif()
    
    if(APPLE_IOS)
        set(CROSS ${CROSS}
            -DIOS_PLATFORM=${IOS_PLATFORM})
    endif()
endif()

# Set hardcoded path guesses for various platforms
if (UNIX AND NOT EMSCRIPTEN)
  set(PROJECT_DEP_SEARCH_PATH ${PROJECT_DEP_SEARCH_PATH} /usr/local)
  # Ubuntu 11.10 has an inconvenient path to OpenGL libraries
  set(PROJECT_DEP_SEARCH_PATH ${PROJECT_DEP_SEARCH_PATH} /usr/lib/${CMAKE_SYSTEM_PROCESSOR}-linux-gnu)
endif ()

# give guesses as hints to the find_package calls
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${PROJECT_DEP_SEARCH_PATH})
set(CMAKE_FRAMEWORK_PATH ${CMAKE_FRAMEWORK_PATH} ${PROJECT_DEP_SEARCH_PATH})
set(DEP_BUILD_TYPE ${CMAKE_BUILD_TYPE})

if(PROJECT_BUILD_DEPENDENCIES AND NOT EXISTS ${PROJECTDEPS_PATH})
    set(PROJECTDEPS_SHARED TRUE)
    if(PROJECT_STATIC OR MSVC)
        # freetype and Glslang does not like shared build on MSVC and it generally eases distribution there
        set(PROJECTDEPS_SHARED FALSE)
    endif()

    if (NOT DEBUG)
        set(DEP_BUILD_TYPE "Release")
    endif(NOT DEBUG)

    set(BUILD_COMMAND_OPTS --target install --config ${DEP_BUILD_TYPE})

    if(MSVC OR EMSCRIPTEN OR MINGW) # other platforms ship zlib
        message(STATUS "Building zlib")
        file(DOWNLOAD 
            http://zlib.net/zlib-1.2.11.tar.gz
            ${CMAKE_BINARY_DIR}/zlib-1.2.11.tar.gz 
            EXPECTED_MD5 1c9f62f0778697a09d36121ead88e08e)
        execute_process(COMMAND ${CMAKE_COMMAND} 
            -E tar xf zlib-1.2.11.tar.gz WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
        execute_process(COMMAND ${CMAKE_COMMAND}
            -DCMAKE_INSTALL_PREFIX=${PROJECTDEPS_PATH}
            -DCMAKE_BUILD_TYPE=${DEP_BUILD_TYPE}
            -DBUILD_SHARED_LIBS=${PROJECTDEPS_SHARED}
            -G ${CMAKE_GENERATOR}
            -DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}
            ${CROSS}
            ${CMAKE_BINARY_DIR}/zlib-1.2.11
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/zlib-1.2.11)
        execute_process(COMMAND ${CMAKE_COMMAND} 
            --build ${CMAKE_BINARY_DIR}/zlib-1.2.11 ${BUILD_COMMAND_OPTS})
    endif()

    message(STATUS "Building ZZIPlib")
    file(DOWNLOAD
        https://github.com/paroj/ZZIPlib/archive/master.tar.gz
        ${CMAKE_BINARY_DIR}/ZZIPlib-master.tar.gz)
    execute_process(COMMAND ${CMAKE_COMMAND}
        -E tar xf ZZIPlib-master.tar.gz WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    execute_process(COMMAND ${CMAKE_COMMAND}
        -DCMAKE_INSTALL_PREFIX=${PROJECTDEPS_PATH}
        -DCMAKE_BUILD_TYPE=${DEP_BUILD_TYPE}
        -DZLIB_ROOT=${PROJECTDEPS_PATH}
        -DBUILD_SHARED_LIBS=${PROJECTDEPS_SHARED}
        -G ${CMAKE_GENERATOR}
        -DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}
        ${CROSS}
        ${CMAKE_BINARY_DIR}/ZZIPlib-master
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/ZZIPlib-master)
    execute_process(COMMAND ${CMAKE_COMMAND} 
        --build ${CMAKE_BINARY_DIR}/ZZIPlib-master ${BUILD_COMMAND_OPTS})
    
    message(STATUS "Building freetype")
    file(DOWNLOAD
        https://download.savannah.gnu.org/releases/freetype/freetype-2.9.tar.gz
        ${CMAKE_BINARY_DIR}/freetype-2.9.tar.gz)
    execute_process(COMMAND ${CMAKE_COMMAND}
        -E tar xf freetype-2.9.tar.gz WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    # patch toolchain for iOS
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/CMake/toolchain/ios.toolchain.xcode.cmake
        freetype-2.9/builds/cmake/iOS.cmake
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    execute_process(COMMAND ${CMAKE_COMMAND}
        -DCMAKE_INSTALL_PREFIX=${PROJECTDEPS_PATH}
        -DCMAKE_BUILD_TYPE=${DEP_BUILD_TYPE}
        -DBUILD_SHARED_LIBS=${PROJECTDEPS_SHARED}
        -DWITH_PNG=OFF
        -DWITH_BZip2=OFF # tries to use it on iOS otherwise
        # workaround for broken iOS toolchain in freetype
        -DPROJECT_SOURCE_DIR=${CMAKE_BINARY_DIR}/freetype-2.9
        ${CROSS}
        -G ${CMAKE_GENERATOR}
        -DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}
        ${CMAKE_BINARY_DIR}/freetype-2.9
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/freetype-2.9/objs)
    execute_process(COMMAND ${CMAKE_COMMAND}
        --build ${CMAKE_BINARY_DIR}/freetype-2.9/objs ${BUILD_COMMAND_OPTS})

    if(MSVC OR MINGW) # other platforms dont need this
        message(STATUS "Building SDL2")
        file(DOWNLOAD
            https://libsdl.org/release/SDL2-2.0.8.tar.gz
            ${CMAKE_BINARY_DIR}/SDL2-2.0.8.tar.gz)
        execute_process(COMMAND ${CMAKE_COMMAND} 
            -E tar xf SDL2-2.0.8.tar.gz WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
        execute_process(COMMAND ${CMAKE_COMMAND}
            -E make_directory ${CMAKE_BINARY_DIR}/SDL2-build)
        execute_process(COMMAND ${CMAKE_COMMAND}
            -DCMAKE_INSTALL_PREFIX=${PROJECTDEPS_PATH}
            -DCMAKE_BUILD_TYPE=${DEP_BUILD_TYPE}
            -DSDL_STATIC=FALSE
            -G ${CMAKE_GENERATOR}
            -DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}
            ${CROSS}
            ${CMAKE_BINARY_DIR}/SDL2-2.0.8
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/SDL2-build)
        execute_process(COMMAND ${CMAKE_COMMAND}
            --build ${CMAKE_BINARY_DIR}/SDL2-build ${BUILD_COMMAND_OPTS})
    endif()

    if(Vulkan_FOUND OR MSVC OR MINGW) # other platforms dont need this
        message(STATUS "Building Glslang")
        file(DOWNLOAD
            https://github.com/KhronosGroup/glslang/archive/master-tot.tar.gz
            ${CMAKE_BINARY_DIR}/Glslang-master.tar.gz)
        execute_process(COMMAND ${CMAKE_COMMAND}
            -E tar xf Glslang-master.tar.gz WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
        if(NOT USE_INBUILD_GLSLANG)
            execute_process(COMMAND ${CMAKE_COMMAND}
                -DCMAKE_INSTALL_PREFIX=${PROJECTDEPS_PATH}
                -DCMAKE_INSTALL_LIBDIR=${PROJECTDEPS_PATH}
                -DCMAKE_BUILD_TYPE=${DEP_BUILD_TYPE}
                -DBUILD_SHARED_LIBS=${PROJECTDEPS_SHARED}
                -DENABLE_GLSLANG_INSTALL=ON
                -G ${CMAKE_GENERATOR}
                -DCMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}
                ${CROSS}
                ${CMAKE_BINARY_DIR}/glslang-master-tot
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/glslang-master-tot)
            execute_process(COMMAND ${CMAKE_COMMAND} 
                --build ${CMAKE_BINARY_DIR}/glslang-master-tot ${BUILD_COMMAND_OPTS})
        endif(USE_INBUILD_GLSLANG)
    endif()
endif()

#######################################################################
# Core dependencies
#######################################################################

# Find zlib
find_package(ZLIB)
macro_log_feature(ZLIB "zlib" "Simple data compression library" "http://www.zlib.net" FALSE "" "")
AddInterfaceLibrary(ZLIB)
IF (ZLIB_USE_BUILDIN)
    SET(ZLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dep/zlib )
    SET(ZLIB_LIBRARIES zlib)
ENDIF(ZLIB_USE_BUILDIN)

if (ZLIB_FOUND)
  # Find zziplib
  find_package(ZZip)
  macro_log_feature(ZZip "zziplib" "Extract data from zip archives" "http://zziplib.sourceforge.net" FALSE "" "")
  AddInterfaceLibrary(ZZip)
endif ()

# Find FreeImage
find_package(FreeImage)
macro_log_feature(FreeImage "freeimage" "Support for commonly used graphics image formats" "http://freeimage.sourceforge.net" FALSE "" "")
AddInterfaceLibrary(FreeImage)

# Find FreeType
find_package(Freetype)
macro_log_feature(FREETYPE "freetype" "Portable font engine" "http://www.freetype.org" FALSE "" "")
AddInterfaceLibrary(FREETYPE)

# Find X11
if (UNIX AND NOT APPLE AND NOT ANDROID AND NOT EMSCRIPTEN)
  find_package(X11)
  macro_log_feature(X11 "X11" "X Window system" "http://www.x.org" TRUE "" "")
  find_library(XAW_LIBRARY NAMES Xaw Xaw7 PATHS ${PROJECT_DEP_SEARCH_PATH} ${DEP_LIB_SEARCH_DIR} ${X11_LIB_SEARCH_PATH})
  macro_log_feature(XAW_LIBRARY "Xaw" "X11 Athena widget set" "http://www.x.org" TRUE "" "")
  mark_as_advanced(XAW_LIBRARY)
endif ()

  
if (USE_INBUILD_GLSLANG)
    option(ENABLE_SPVREMAPPER "Enables building of SPVRemapper" ON)
    option(ENABLE_AMD_EXTENSIONS "Enables support of AMD-specific extensions" ON)
    option(ENABLE_GLSLANG_BINARIES "Builds glslangValidator and spirv-remap" OFF)
    option(ENABLE_NV_EXTENSIONS "Enables support of Nvidia-specific extensions" ON)
    option(ENABLE_HLSL "Enables HLSL input support" ON)
    option(ENABLE_OPT "Enables spirv-opt capability if present" OFF) 
    
    findpkg_begin(Glslang)

    set(Glslang_INCLUDE_DIR ${CMAKE_BINARY_DIR}/glslang-master-tot ${CMAKE_BINARY_DIR}/glslang-master-tot/glslang/Public ${CMAKE_BINARY_DIR}/glslang-master-tot/SPIRV)
    set(Glslang_LIBRARY glslang SPIRV SPVRemapper HLSL OSDependent)

    findpkg_finish(Glslang)

    message("inc dir: ${Glslang_INCLUDE_DIR}")
    message("inc lib: ${Glslang_LIBRARY}")

    if (NOT Glslang_FOUND)
        message(FATAL_ERROR "Missing Glslang")
    endif(NOT Glslang_FOUND)

    if(ENABLE_AMD_EXTENSIONS)
        add_definitions(-DAMD_EXTENSIONS)
    endif(ENABLE_AMD_EXTENSIONS)
    
    if(ENABLE_NV_EXTENSIONS)
        add_definitions(-DNV_EXTENSIONS)
    endif(ENABLE_NV_EXTENSIONS)
    
    if(ENABLE_HLSL)
        add_definitions(-DENABLE_HLSL)
    endif(ENABLE_HLSL)

    SET(CMAKE_INSTALL_LIBDIR ${LibName})

    #ADD_DEFINITIONS(-DENABLE_GLSLANG_INSTALL=ON)
    add_subdirectory(${CMAKE_BINARY_DIR}/glslang-master-tot/glslang)
    add_subdirectory(${CMAKE_BINARY_DIR}/glslang-master-tot/OGLCompilersDLL) 
    add_subdirectory(${CMAKE_BINARY_DIR}/glslang-master-tot/SPIRV)
    if(ENABLE_HLSL)
        add_subdirectory(${CMAKE_BINARY_DIR}/glslang-master-tot/hlsl)
    endif(ENABLE_HLSL)

    AddInterfaceLibrary(Glslang)
elseif (Vulkan_FOUND)
  #set(CMAKE_FIND_ROOT_PATH "${CMAKE_BINARY_DIR}/glslang-master-tot" "${CMAKE_FIND_ROOT_PATH}")
  
  # Find Glslang
  find_package(Glslang)
  macro_log_feature(Glslang "Glslang" "Glslang SDK needed for building Vulkan driver" "https://github.com/KhronosGroup/glslang/" FALSE "" "")
  AddInterfaceLibrary(Glslang)
endif(USE_INBUILD_GLSLANG)

#######################################################################
# Additional features
#######################################################################

# Find Cg
if (NOT (APPLE_IOS OR WINDOWS_STORE OR WINDOWS_PHONE OR ANDROID OR EMSCRIPTEN))
  find_package(Cg)
  macro_log_feature(Cg "cg" "C for graphics shader language" "http://developer.nvidia.com/object/cg_toolkit.html" FALSE "" "")
endif ()

# OpenEXR
find_package(OpenEXR)
macro_log_feature(OPENEXR "OpenEXR" "Load High dynamic range images" "http://www.openexr.com/" FALSE "" "")

# Python
set(Python_ADDITIONAL_VERSIONS 3.4) # allows using python3 on Ubuntu 14.04
find_package(PythonInterp)
find_package(PythonLibs)
macro_log_feature(PYTHONLIBS "Python" "Language bindings to use PROJECT from Python" "http://www.python.org/" FALSE "" "")

#######################################################################
# Tools
#######################################################################

find_package(Doxygen)
macro_log_feature(DOXYGEN "Doxygen" "Tool for building API documentation" "http://doxygen.org" FALSE "" "")

# Find Softimage SDK
find_package(Softimage)
macro_log_feature(Softimage "Softimage" "Softimage SDK needed for building XSIExporter" FALSE "6.0" "")

#######################################################################
# Tests
#######################################################################

# now see if we have a buildable Dependencies package in
# the source tree. If so, include that, and it will take care of
# setting everything up, including overriding any of the above
# findings.
set(PROJECTDEPS_RUNTIME_OUTPUT ${PROJECT_RUNTIME_OUTPUT})
if (EXISTS "${CMAKE_SOURCE_DIR}/Dependencies/CMakeLists.txt")
  add_subdirectory(Dependencies)
endif ()

# provide option to install dependencies on Windows
include(InstallDependencies)

#######################################################################
# Find all necessary
#######################################################################

#######################################################################
# Boost
#######################################################################
include(ConfigureBoost)
