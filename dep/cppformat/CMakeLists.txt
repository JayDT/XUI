include(CheckSymbolExists)

if (WIN32)
  check_symbol_exists(open io.h HAVE_OPEN)
else ()
  check_symbol_exists(open fcntl.h HAVE_OPEN)
endif ()

set(FMT_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/cppformat/format.h
  ${CMAKE_CURRENT_SOURCE_DIR}/cppformat/format.cc)

if (HAVE_OPEN)
  set(FMT_SOURCES ${FMT_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/cppformat/posix.h
    ${CMAKE_CURRENT_SOURCE_DIR}/cppformat/posix.cc)
endif()

add_library(cppformat STATIC ${FMT_SOURCES})

target_include_directories(cppformat
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(cppformat
    PROPERTIES
      FOLDER
        "dep")

MESSAGE(${LibSTATUS} "cppformat")
