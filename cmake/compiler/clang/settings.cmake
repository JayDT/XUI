# Set build-directive (used in core to tell which buildtype we used)
add_definitions(-D_BUILD_DIRECTIVE="${CMAKE_BUILD_TYPE}")

if(WITH_WARNINGS)
  set(WARNING_FLAGS "-W -Wall -Wextra -Winit-self -Wfatal-errors -Wno-mismatched-tags")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} -Woverloaded-virtual")
  message(STATUS "Clang: All warnings enabled")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3 -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O3")
message(STATUS "Clang: Debug-flags set (-g3)")

set(DISABLED_WARNINGS " -Wno-tautological-compare -Wno-undefined-bool-conversion -Wno-pointer-bool-conversion -Wno-switch -Wno-undefined-var-template -Wno-absolute-value -Wno-inconsistent-missing-override -Wno-narrowing -Wno-deprecated-register")

# -Wno-narrowing needed to suppress a warning in g3d
# -Wno-deprecated-register is needed to suppress 185 gsoap warnings on Unix systems. -gdwarf-3
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DISABLED_WARNINGS} -std=c++1z -march=native -fPIC")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3 -O0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O0")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3 -O0")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${DISABLED_WARNINGS} -g3 -O0 -march=native -std=c++1z -DDEBUG=1 -fPIC")
  message(STATUS "Clang: Debug node With Debug-flags set (-g3)")
endif()

if (BUILD_SHARED_LIBS)
  # -fPIC is needed to allow static linking in shared libs.
  # -fvisibility=hidden sets the default visibility to hidden to prevent exporting of all symbols.
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -fvisibility=hidden")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fvisibility=hidden")

  # --no-undefined to throw errors when there are undefined symbols
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --no-undefined")

  message(STATUS "Clang: Disallow undefined symbols")
endif()

if (PCH)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(GLOBAL_CXX_FLAGS ${CMAKE_CXX_FLAGS_DEBUG})
    else()
        set(GLOBAL_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    endif()
endif()

add_definitions(-DSYSCONFDIR="${SYSCONFDIR}")