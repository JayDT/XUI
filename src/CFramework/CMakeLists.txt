SET(MODUL_NAME "CFramework")
SET(MODUL_LINKAGE "STATIC")
SET(MODUL_LINK_LIBS "")
SET(MODUL_LINK_FLAGS "")
SET(LibCFrameworkSharedLinkage 1)

IF (LibCFrameworkSharedLinkage)
    ADD_DEFINITIONS(-DTC_API_EXPORT_CFRAMEWORK=1)
    SET(MODUL_LINKAGE "SHARED")

    list(APPEND MODUL_LINK_LIBS
       boost
       cppformat
       g3dlite
	   libjpeg
	   libpng	   
       ZLIB
       ${CURL_LIBRARY}
       ${OPENSSL_LIBRARIES}
     )
    
    list(APPEND MODUL_LINK_LIBS
        libCallStack
        MetaRuntime
     )

    IF(WIN32)
        LINK_DIRECTORIES("${dep}\\lib\\${PLATFORM_NAME}_${CMAKE_BUILD_TYPE}\\")
    ENDIF()
ENDIF(LibCFrameworkSharedLinkage)

# Glob only and not recurse, there are other libs for that
file(GLOB_RECURSE MODUL_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.c *.cc)
file(GLOB_RECURSE MODUL_HEADER_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.hpp *.h)

LIST(APPEND MODUL_SRCS ${MODUL_HEADER_SRCS})
group_source(${MODUL_SRCS})
setup_hlsl(${MODUL_SRCS})

SET(MODUL_HEADER_SRCS "")
file(GLOB_RECURSE MODUL_HEADER_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        *collection.h
        *events.h
        *System/*.h)

meta_parser_prebuild(
    TARGET ${MODUL_NAME}
    GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/.Assembly"
    SOURCE_ROOT "${CMAKE_CURRENT_LIST_DIR}"
    HEADER_FILES ${MODUL_HEADER_SRCS}
    MODULE_HEADER "${MODUL_NAME}AssemblyModul.h"
    OUT_MODULE_SOURCE META_MODULE_SOURCE
    OUT_GENERATED_FILES META_GENERATED_FILES
    OUT_INC META_GENERATED_HEADERS
    OUT_SRC META_GENERATED_SOURCES
)

LIST(APPEND MODUL_SRCS ${META_GENERATED_HEADERS})
LIST(APPEND MODUL_SRCS ${META_GENERATED_SOURCES})

AppendDefaultIncludeDir(${CMAKE_CURRENT_SOURCE_DIR}/standard)
include_directories(
 ${CMAKE_CURRENT_BINARY_DIR}/.Assembly
 ${CMAKE_SOURCE_DIR}/dep/jpeglib
 ${CMAKE_SOURCE_DIR}/dep/libpng
)

CreateLibrary("CFramework" "${MODUL_SRCS}" "${MODUL_LINKAGE}" "${MODUL_LINK_LIBS}" "${MODUL_LINK_FLAGS}")
CreateCxxPch("CFramework" "${CMAKE_CURRENT_SOURCE_DIR}/StdAfx.h" "${CMAKE_CURRENT_SOURCE_DIR}/StdAfx.cpp")
add_dependencies(CFramework MetaParser)

meta_parser_build(
    TARGET ${MODUL_NAME}
    GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/.Assembly"
    SOURCE_ROOT "${CMAKE_CURRENT_LIST_DIR}"
    GENERATED_FILES ${META_GENERATED_FILES}
    SOURCE_FILE "${CMAKE_CURRENT_BINARY_DIR}/.Assembly/Assembly_${MODUL_NAME}.dsc"
    MODULE_HEADER "${MODUL_NAME}AssemblyModul.h"
    MODULE_SOURCE_FILE ${META_MODULE_SOURCE}
    HEADER_FILES ${MODUL_HEADER_SRCS}
    PCH_NAME "${CMAKE_CURRENT_SOURCE_DIR}/StdAfx" 
    PARSER_EXECUTABLE ${MetaParserExecutable}
)

set_target_properties(CFramework
    PROPERTIES
      FOLDER
        "Source")