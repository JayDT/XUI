set(MSVC_EXPECTED_VERSION 19.0.0) # MSVC Update 3

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS MSVC_EXPECTED_VERSION)
  message(FATAL_ERROR "MSVC: XUI requires version ${MSVC_EXPECTED_VERSION} (MSVC 2017) to build but found ${CMAKE_CXX_COMPILER_VERSION}")
endif()

# Set build-directive (used in core to tell which buildtype we used)
# msbuild/devenv don't set CMAKE_MAKE_PROGRAM, you can choose build type from a dropdown after generating projects
if("${CMAKE_MAKE_PROGRAM}" MATCHES "MSBuild")
  add_definitions(-D_BUILD_DIRECTIVE="$(ConfigurationName)")
else()
  # while all make-like generators do (nmake, ninja)
  add_definitions(-D_BUILD_DIRECTIVE="${CMAKE_BUILD_TYPE}")
endif()

#For HotSwap Script Loaders
add_definitions(-D_RUNTIME_PATH_="${BIN_DIR}")

# Setup MultiThreaded compiling on msvc
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP /Zi")
set(CMAKE_C_FLAGS_DEBUG "/DWIN32 /D_WINDOWS /W3 /Zi /MP /Ox /Oi /Ot /Ob2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /std:c++latest")

message(STATUS "MSVC: Enabled PDB generators")

# Setup Edit And Continue
if(PLATFORM EQUAL 64)
    # This definition is necessary to work around a bug with Intellisense described
    # here: http://tinyurl.com/2cb428.  Syntax highlighting is important for proper
    # debugger functionality.
    add_definitions(-D_WIN64)
    message(STATUS "MSVC: 64-bit platform, enforced -D_WIN64 parameter")

    set(CMAKE_CXX_FLAGS_DEBUG "/Zi")
    if (EAC)
        message(STATUS "MSVC: Edit&Continue not supported")
    endif (EAC)
else()

    # mark 32 bit executables large address aware so they can use > 2GB address space
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
    message(STATUS "MSVC: Enabled large address awareness")

    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /SAFESEH:NO")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /SAFESEH:NO")
    set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} /SAFESEH:NO")  
    message(STATUS "MSVC: Disabled Safe Exception Handlers for debug builds")

    # Setup Fast link for Debug
    if (WFL)
        message(STATUS "MSVC: Enable Fast Link For Debug (HotSwap Library not supported)")
        #set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /Debug:FastLink")
        #set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /Debug:FastLink")
    endif(WFL)
    
    if (EAC)
        message(STATUS "MSVC: Edit&Continue support")
        set(CMAKE_CXX_FLAGS_DEBUG "/ZI")

        if (NOT PLATFORM EQUAL 64 AND EAC)
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Gy ")
        else()
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Gy-")
        endif()
    else()
        set(CMAKE_CXX_FLAGS_DEBUG "/Zi")
    endif (EAC)
endif()

#Enable extended object support for debug compiles on X64 (not required on X86)
if((PLATFORM EQUAL 64) OR (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.0.23026.0) OR BUILD_SHARED_LIBS)
  # Enable extended object support
  message(STATUS "MSVC: Enabled extended object-support for debug-compiles")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
  message(STATUS "MSVC: Enabled increased number of sections in object files")
endif()

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Ob0") #/Oy- /Ot /GS- /EHa /GR /GF-

#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm1024")
#    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zm1024")

if (DEBUG GREATER 1)
    message(STATUS "MSVC: Debug mode: Full Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG /MDd  /RTC1 /fp:except")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Ob0") #/Oy- /Ot /GS- /EHa /GR /GF-
else() # Prefer for runtime speed partial debug mode
    message(STATUS "MSVC: Debug mode: Partial Debug")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -D_NO_DEBUG_HEAP=1 -DNDEBUG /MD /fp:except-")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_NO_DEBUG_HEAP=1 -DNDEBUG /MD /fp:except-")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Ob0  /GS-") #/Oy- /Ot /GS- /EHa /GR /GF-
endif()

# Define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES - eliminates the warning by changing the strcpy call to strcpy_s, which prevents buffer overruns
add_definitions(-D_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES)
message(STATUS "MSVC: Overloaded standard names")

# Ignore warnings about older, less secure functions
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
message(STATUS "MSVC: Disabled NON-SECURE warnings")

#Ignore warnings about POSIX deprecation
add_definitions(-D_CRT_NONSTDC_NO_WARNINGS)
message(STATUS "MSVC: Disabled POSIX warnings")

add_definitions(-D_BUILD_PLATFORM_TOOL_SET=$(PlatformToolsetVersion))

# disable warnings in Visual Studio 8 and above if not wanted
if(NOT WITH_WARNINGS)
  if(MSVC AND NOT CMAKE_GENERATOR MATCHES "Visual Studio 7")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4996 /wd4244 /wd4985 /wd4619 /wd4244")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /wd4996 /wd4244 /wd4985 /wd4619 /wd4244")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996 /wd4244 /wd4985 /wd4619 /wd4244")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4996 /wd4244 /wd4985 /wd4619 /wd4244")
    message(STATUS "MSVC: Disabled generic compiletime warnings")
  endif()
endif()

# /Zc:throwingNew.
# When you specify Zc:throwingNew on the command line, it instructs the compiler to assume
# that the program will eventually be linked with a conforming operator new implementation,
# and can omit all of these extra null checks from your program.
# http://blogs.msdn.com/b/vcblog/archive/2015/08/06/new-in-vs-2015-zc-throwingnew.aspx
if(NOT (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.0.23026.0))
  # makes this flag a requirement to build TC at all
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:throwingNew")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  /Zc:throwingNew")
endif()

if (LibSharedLinkage OR LibGameLinkage OR SD2Linkage)
  # C4251: needs to have dll-interface to be used by clients of class '...'
  # C4275: non dll-interface class ...' used as base for dll-interface class '...'
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
  message(STATUS "MSVC: Setup Dynamic linking checks")
endif()

# Specify the maximum PreCompiled Header memory allocation limit
# Fixes a compiler-problem when using PCH - the /Ym flag is adjusted by the compiler in MSVC2012, hence we need to set an upper limit with /Zm to avoid discrepancies)
# (And yes, this is a verified , unresolved bug with MSVC... *sigh*)
string(REGEX REPLACE "/Zm[0-9]+ *" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
string(REGEX REPLACE "/Zm[0-9]+ *" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm1024")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zm1024")

# Enable and treat as errors the following warnings to easily detect virtual function signature failures:
# 'function' : member function does not override any base class virtual member function
# 'virtual_function' : no override available for virtual member function from base 'class'; function is hidden
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /we4263 /we4264")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /we4263 /we4264")

add_definitions(-DSYSCONFDIR="${SYSCONFDIR}")

SET(GLOBAL_LINK_FLAGS "/MANIFEST:NO /OPT:NOLBR")

if(EAC)
    message(STATUS "MSVC: Configured Linker Edit and Continue")
    SET(GLOBAL_LINK_FLAGS "${GLOBAL_LINK_FLAGS} /SAFESEH:NO")
endif(EAC)

if (NOT DEBUG AND INSTEX)
    include(CheckSSEFeatureSet)
    message(STATUS "Compiler Flags ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GLOBAL_LINK_FLAGS}") 
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${GLOBAL_LINK_FLAGS}") 
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${GLOBAL_LINK_FLAGS}") 
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${GLOBAL_LINK_FLAGS}") 
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} ${GLOBAL_LINK_FLAGS}") 
set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} ${GLOBAL_LINK_FLAGS}") 
