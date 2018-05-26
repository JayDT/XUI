# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# InstallRequiredSystemLibraries
# ------------------------------
#
# Include this module to search for compiler-provided system runtime
# libraries and add install rules for them.  Some optional variables
# may be set prior to including the module to adjust behavior:
#
# ``CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS``
#   Specify additional runtime libraries that may not be detected.
#   After inclusion any detected libraries will be appended to this.
#
# ``CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP``
#   Set to TRUE to skip calling the :command:`install(PROGRAMS)` command to
#   allow the includer to specify its own install rule, using the value of
#   ``CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS`` to get the list of libraries.
#
# ``CMAKE_INSTALL_DEBUG_LIBRARIES``
#   Set to TRUE to install the debug runtime libraries when available
#   with MSVC tools.
#
# ``CMAKE_INSTALL_DEBUG_LIBRARIES_ONLY``
#   Set to TRUE to install only the debug runtime libraries with MSVC
#   tools even if the release runtime libraries are also available.
#
# ``CMAKE_INSTALL_UCRT_LIBRARIES``
#   Set to TRUE to install the Windows Universal CRT libraries for
#   app-local deployment (e.g. to Windows XP).  This is meaningful
#   only with MSVC from Visual Studio 2015 or higher.
#
#   One may set a ``CMAKE_WINDOWS_KITS_10_DIR`` *environment variable*
#   to an absolute path to tell CMake to look for Windows 10 SDKs in
#   a custom location.  The specified directory is expected to contain
#   ``Redist/ucrt/DLLs/*`` directories.
#
# ``CMAKE_INSTALL_MFC_LIBRARIES``
#   Set to TRUE to install the MSVC MFC runtime libraries.
#
# ``CMAKE_INSTALL_OPENMP_LIBRARIES``
#   Set to TRUE to install the MSVC OpenMP runtime libraries
#
# ``CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION``
#   Specify the :command:`install(PROGRAMS)` command ``DESTINATION``
#   option.  If not specified, the default is ``bin`` on Windows
#   and ``lib`` elsewhere.
#
# ``CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS``
#   Set to TRUE to disable warnings about required library files that
#   do not exist.  (For example, Visual Studio Express editions may
#   not provide the redistributable files.)
#
# ``CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT``
#   Specify the :command:`install(PROGRAMS)` command ``COMPONENT``
#   option.  If not specified, no such option will be used.

set(_IRSL_HAVE_MSVC FALSE)
foreach(LANG IN ITEMS C CXX Fortran)
  if(CMAKE_${LANG}_COMPILER_ID STREQUAL MSVC)
    set(_IRSL_HAVE_MSVC TRUE)
  endif()
endforeach()

#TODO: linux and lower msvc than 15

if(MSVC)
  file(TO_CMAKE_PATH "$ENV{SYSTEMROOT}" SYSTEMROOT)

  if(CMAKE_CL_64)
    if(MSVC_VERSION GREATER 1599)
      # VS 10 and later:
      set(CMAKE_MSVC_ARCH x64)
    else()
      # VS 9 and earlier:
      set(CMAKE_MSVC_ARCH amd64)
    endif()
  else()
    set(CMAKE_MSVC_ARCH x86)
  endif()

  get_filename_component(devenv_dir "${CMAKE_MAKE_PROGRAM}" PATH)
  get_filename_component(base_dir "${devenv_dir}/../.." ABSOLUTE)

  set(MSVC_REDIST_NAME "")
  set(_MSVCRT_DLL_VERSION "")
  set(_MSVCRT_IDE_VERSION "")
  if(MSVC_VERSION GREATER_EQUAL 2000)
    message(WARNING "MSVC ${MSVC_VERSION} not yet supported.")
  elseif(MSVC_VERSION GREATER_EQUAL 1911)
    set(MSVC_REDIST_NAME VC141)
    set(_MSVCRT_DLL_VERSION 140)
    set(_MSVCRT_IDE_VERSION 15)
  elseif(MSVC_VERSION EQUAL 1910)
    set(MSVC_REDIST_NAME VC150)
    set(_MSVCRT_DLL_VERSION 140)
    set(_MSVCRT_IDE_VERSION 15)
  elseif(MSVC_VERSION EQUAL 1900)
    set(MSVC_REDIST_NAME VC140)
    set(_MSVCRT_DLL_VERSION 140)
    set(_MSVCRT_IDE_VERSION 14)
  elseif(MSVC_VERSION EQUAL 1800)
    set(MSVC_REDIST_NAME VC120)
    set(_MSVCRT_DLL_VERSION 120)
    set(_MSVCRT_IDE_VERSION 12)
  elseif(MSVC_VERSION EQUAL 1700)
    set(MSVC_REDIST_NAME VC110)
    set(_MSVCRT_DLL_VERSION 110)
    set(_MSVCRT_IDE_VERSION 11)
  elseif(MSVC_VERSION EQUAL 1600)
    set(MSVC_REDIST_NAME VC100)
    set(_MSVCRT_DLL_VERSION 100)
    set(_MSVCRT_IDE_VERSION 10)
  endif()

  if(_MSVCRT_DLL_VERSION)
    set(v "${_MSVCRT_DLL_VERSION}")
    set(vs "${_MSVCRT_IDE_VERSION}")

    # Find the runtime library redistribution directory.
    if(vs VERSION_LESS 15 AND DEFINED MSVC${vs}_REDIST_DIR AND EXISTS "${MSVC${vs}_REDIST_DIR}")
      set(MSVC_RUNTIME_INCLUDE_DIR "${MSVC${vs}_REDIST_DIR}") # use old cache entry
    endif()
    if(NOT vs VERSION_LESS 15)
      set(_vs_redist_paths "")
      cmake_host_system_information(RESULT _vs_dir QUERY VS_${vs}_DIR) # undocumented query
      if(IS_DIRECTORY "${_vs_dir}")
        file(GLOB _vs_redist_paths "${_vs_dir}/VC/Tools/MSVC/*")
      endif()
      unset(_vs_dir)
    else()
      get_filename_component(_vs_dir
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\${vs}.0;InstallDir]" ABSOLUTE)
      if(IS_DIRECTORY "${_vs_dir}")
        file(GLOB _vs_redist_paths "${_vs_dir}/VC/*")
      endif()
      unset(_vs_dir)
    endif()

    #message("redist dir ${_vs_redist_paths}")
    find_path(MSVC_RUNTIME_INCLUDE_DIR NAMES crtdefs.h PATHS "${_vs_redist_paths}/include")
    #message("redist dir ${MSVC_RUNTIME_INCLUDE_DIR}")

    SET(includes ${MSVC_RUNTIME_INCLUDE_DIR})

    if(CMAKE_INSTALL_UCRT_LIBRARIES AND NOT vs VERSION_LESS 14)
      # Find the Windows Kits directory.
      get_filename_component(windows_kits_dir
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot10]" ABSOLUTE)
      set(programfilesx86 "ProgramFiles(x86)")
      find_path(WINDOWS_KITS_INCLUDE_DIR NAMES corecrt_memory.h
        PATHS
        $ENV{CMAKE_WINDOWS_KITS_10_DIR}
        "${windows_kits_dir}"
        "$ENV{ProgramFiles}/Windows Kits/10"
        "$ENV{${programfilesx86}}/Windows Kits/10"
        )

        list(APPEND includes ${WINDOWS_KITS_INCLUDE_DIR})
    endif()
  endif()

  set(CMAKE_INSTALL_SYSTEM_RUNTIME_INCLUDES
      ${CMAKE_INSTALL_SYSTEM_RUNTIME_INCLUDES} ${includes})

endif()



