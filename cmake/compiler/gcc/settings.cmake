# Set build-directive (used in core to tell which buildtype we used)
add_definitions(-D_BUILD_DIRECTIVE="${CMAKE_BUILD_TYPE}")

set(GCC_EXPECTED_VERSION 4.9.0)

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS GCC_EXPECTED_VERSION)
  message(FATAL_ERROR "GCC: TCore requires version ${GCC_EXPECTED_VERSION} to build but found ${CMAKE_CXX_COMPILER_VERSION}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z") # -ftime-report
message(STATUS "GCC: Enabled c++17 support")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
message(STATUS "GCC: Enabled C99(with gnu99) support")

if(PLATFORM EQUAL 32)
  # Required on 32-bit systems to enable SSE2 (standard on x64)
  set(SSE_FLAGS "-msse2 -mfpmath=sse")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SSE_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SSE_FLAGS}")
endif()
add_definitions(-DHAVE_SSE2 -D__SSE2__)
message(STATUS "GCC: SFMT enabled, SSE2 flags forced")

if( WITH_WARNINGS )
  set(WARNING_FLAGS "-W -Wall -Wextra -Winit-self -Winvalid-pch -Wfatal-errors")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNING_FLAGS} -Woverloaded-virtual")
  message(STATUS "GCC: All warnings enabled")
endif()

if( DEBUG )
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb3 -O0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3 -O0")

  set(CMAKE_C_FLAGS_DEBUG " -ggdb3 -O0")
  set(CMAKE_CXX_FLAGS_DEBUG " -ggdb3 -O0")
  
  message(STATUS "GCC: Debug-flags set (-ggdb3)")
else()
 
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb3 -O2 -minline-all-stringops")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3 -O2 -minline-all-stringops")
endif()

if ( PCH )
	message(STATUS "GCC: GCH Warning enabled")
	set(GLOBAL_CXX_FLAGS "${GLOBAL_CXX_FLAGS} -Wall -Winvalid-pch -Wdeprecated")
endif( PCH )

SET(GLOBAL_LINK_FLAGS "-ldl -lssl -lcrypt -pthread -rdynamic")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -Wno-attributes")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wno-attributes")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fPIC -Wno-attributes")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC -Wno-attributes")

message(STATUS "GCC: Enabled shared linking")

# Set build-directive (used in core to tell which buildtype we used)
add_definitions(-DSYSCONFDIR="${SYSCONFDIR}")
