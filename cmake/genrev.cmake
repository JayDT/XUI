# Copyright (C) 2008-2012 Trinity <http://www.trinitycore.org/>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

if(NOT BUILDDIR)
  # Workaround for funny MSVC behaviour - this segment is only used when using cmake gui
  set(BUILDDIR ${CMAKE_BINARY_DIR})
endif()

include(${CMAKE_SOURCE_DIR}/cmake/macros/EnsureVersion.cmake)

set(_REQUIRED_GIT_VERSION "1.7")

find_program(_GIT_EXEC
  NAMES
    git git.cmd
  HINTS
    ENV PATH
  DOC "git installation path"
)

if(_GIT_EXEC)
  execute_process(
    COMMAND "${_GIT_EXEC}" --version
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE _GIT_VERSION
    ERROR_QUIET
  )

  # make sure we're using minimum the required version of git, so the "dirty-testing" will work properly
  ensure_version( "${_REQUIRED_GIT_VERSION}" "${_GIT_VERSION}" _GIT_VERSION_OK)
endif()

if(_GIT_VERSION_OK)
  execute_process(
    COMMAND "${_GIT_EXEC}" show -s --format=oneline
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE rev_info
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )

  execute_process(
    COMMAND "${_GIT_EXEC}" show -s --format=%ci
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE rev_date
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )

    # Also retrieve branch name
  execute_process(
      COMMAND "${GIT_EXECUTABLE}" rev-parse --abbrev-ref HEAD
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      OUTPUT_VARIABLE rev_branch
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
  )
else()
  message("")
  message(STATUS "WARNING - Missing or outdated git - did you forget to install a recent version?")
  message(STATUS "WARNING - Observe that for revision hash/date to work you need at least version ${_REQUIRED_GIT_VERSION}")
endif()

# Last minute check - ensure that we have a proper revision
# If everything above fails (means the user has erased the git revision control directory or removed the origin/HEAD tag)
if(NOT rev_info)
  # No valid ways available to find/set the revision/hash, so let's force some defaults
  message(STATUS "WARNING - Missing repository tags - you may need to pull tags with git fetch -t")
  message(STATUS "WARNING - Continuing anyway - note that the versionstring will be set to 0000-00-00 00:00:00 (Archived)")
  set(rev_date "0000-00-00 00:00:00 +0000")
  set(rev_hash "Archived")
else()
  # Extract information required to build a proper versionstring
  #string(REGEX REPLACE "[0-9]*" "" rev_hash ${rev_info})
endif()

message(STATUS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
set(system_includes "${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}")

# Create the actual revision_data.h file from the above params
#if(NOT "${rev_hash_cached}" MATCHES "${rev_hash}" OR NOT "${rev_branch_cached}" MATCHES "${rev_branch}" OR NOT EXISTS "${CMAKE_BINARY_DIR}/buildin_data.h")
  configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/../buildin_data.h.in.cmake"
    "${CMAKE_BINARY_DIR}/buildin_data.h"
    @ONLY
  )
  set(rev_hash_cached "${rev_hash}" CACHE INTERNAL "Cached commit-hash")
  set(rev_branch_cached "${rev_branch}" CACHE INTERNAL "Cached branch name")
#endif()
