SET(MODUL_LINKAGE "STATIC")
SET(MODUL_LINK_LIBS "")
SET(MODUL_LINK_FLAGS "")

IF (LibMetaRuntimeSharedLinkage)
    ADD_DEFINITIONS(-DTC_API_EXPORT_MetaRuntime=1)
    SET(MODUL_LINKAGE "SHARED")

    list(APPEND MODUL_LINK_LIBS
       boost
       sparsehash
       libCallStack
       ${ZLIB_LIBRARIES}
     )

    IF(WIN32)
        LINK_DIRECTORIES("${dep}\\lib\\${PLATFORM_NAME}_${CMAKE_BUILD_TYPE}\\")
    ENDIF()
ENDIF(LibMetaRuntimeSharedLinkage)

# disable security warnings
add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)

# Glob only and not recurse, there are other libs for that
file(GLOB_RECURSE MODUL_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h *.hpp *.c *.natvis)
list(APPEND ${MODUL_SRCS} " ../Common/Lib/json11.cpp")

group_source(${MODUL_SRCS})

AppendDefaultIncludeDir()
CreateLibrary("MetaRuntime" "${MODUL_SRCS}" "${MODUL_LINKAGE}" "${MODUL_LINK_LIBS}" "${MODUL_LINK_FLAGS}")
CreateCxxPch("MetaRuntime" "${CMAKE_CURRENT_SOURCE_DIR}/Precompiled.h" "${CMAKE_CURRENT_SOURCE_DIR}/Precompiled.cpp")

set_target_properties(MetaRuntime
    PROPERTIES
      FOLDER
        "Reflection")