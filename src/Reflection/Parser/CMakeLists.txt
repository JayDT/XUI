SET(MODUL_LINKAGE "STATIC")
SET(MODUL_LINK_LIBS "")
SET(MODUL_LINK_FLAGS "")
SET(LibMetaParserSharedLinkage 1)

include(InstallRequiredSystemIncludes)

set(system_includes "${CMAKE_INSTALL_SYSTEM_RUNTIME_INCLUDES}")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/common_data.h.in.cmake"
    "${CMAKE_BINARY_DIR}/common_data.h"
    @ONLY
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/Reflection
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/Reflection/Runtime/Common
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/Reflection/Runtime/Common/Lib
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/CFramework
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../dep/cppformat
    ${LLVM_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}
)

list(APPEND MODUL_LINK_LIBS
   boost
   MetaRuntime
   ${LIBCLANG_LIBRARY}
   ${ZLIB_LIBRARIES}
 )

# disable security warnings
add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)

# Glob only and not recurse, there are other libs for that
file(GLOB_RECURSE MODUL_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h *.c *.mustache)

group_source(${MODUL_SRCS})

#AppendDefaultIncludeDir()
CreateExecutable("MetaParser" "${MODUL_SRCS}" "${MODUL_LINK_LIBS}" "${MODUL_LINK_FLAGS}")
CreateCxxPch("MetaParser" "${CMAKE_CURRENT_SOURCE_DIR}/Precompiled.h" "${CMAKE_CURRENT_SOURCE_DIR}/Precompiled.cpp")

message(${LibExSTATUS} "Standard Library Include: ${CMAKE_INSTALL_SYSTEM_RUNTIME_INCLUDES}")

get_property(OUTPUTDIR TARGET MetaParser PROPERTY RUNTIME_OUTPUT_DIRECTORY)
set(MetaParserExecutable "${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE:MetaParser>" CACHE STRING "" FORCE)

# we don't want to copy if we're building in the source dir
if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)

    # list of files for which we add a copy rule
    file(GLOB_RECURSE MODUL_RESOURCE_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.mustache)

    foreach(item IN LISTS MODUL_RESOURCE_SRCS)
        message(${LibExSTATUS} "Add Resource: ${item}")
        add_custom_command(TARGET MetaParser PRE_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy_if_different
                               "${CMAKE_CURRENT_SOURCE_DIR}/${item}"
                               "$<TARGET_FILE_DIR:MetaParser>/${item}"
                           DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${item}")
    endforeach()
endif()

set_target_properties(MetaParser
    PROPERTIES
      FOLDER
        "Reflection")

# copy resources on post build
add_custom_command(TARGET MetaParser POST_BUILD
    # mustache templates directory
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_LIST_DIR}/../../Resources"
        $<TARGET_FILE_DIR:MetaParser>
    # LibClang shared library
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LIBCLANG_SHARED_LIBRARY}
        $<TARGET_FILE_DIR:MetaParser>
)
