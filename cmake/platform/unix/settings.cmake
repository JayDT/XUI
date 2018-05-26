# set default configuration directory
if (PREFIX)
	set(CMAKE_INSTALL_PREFIX "${PREFIX}") 
else()
	set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/XUI_${SERVER_VERSION}") 
endif()

set(BIN_DIR    ${CMAKE_INSTALL_PREFIX}/bin)
set(SYSCONFDIR ${CMAKE_INSTALL_PREFIX}/etc/)
set(LIBS_DIR   ${CMAKE_INSTALL_PREFIX}/lib)

# For Unix systems set the rpath so that libraries are found
set(CMAKE_INSTALL_RPATH    ${LIBS_DIR})
set(CMAKE_INSTALL_NAME_DIR ${LIBS_DIR})

if ( NOT dep)
	set(dep "${CMAKE_SOURCE_DIR}/dep")
endif()

# configure uninstaller
configure_file(
  "${CMAKE_SOURCE_DIR}/cmake/platform/cmake_uninstall.in.cmake"
  "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
  @ONLY
)
message(STATUS "UNIX: Configuring uninstall target")

# create uninstaller target (allows for using "make uninstall")
add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
)
message(STATUS "UNIX: Created uninstall target")

message(STATUS "UNIX: Detected compiler: ${CMAKE_C_COMPILER}")
if(CMAKE_C_COMPILER MATCHES "clang" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/clang/settings.cmake)
elseif(CMAKE_C_COMPILER MATCHES "gcc" OR CMAKE_C_COMPILER MATCHES "cc")
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/gcc/settings.cmake)
elseif(CMAKE_C_COMPILER MATCHES "icc")
  include(${CMAKE_SOURCE_DIR}/cmake/compiler/icc/settings.cmake)
endif()
