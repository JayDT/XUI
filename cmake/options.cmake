SET(CMAKE_COLOR_MAKEFILE   ON)
SET(CMAKE_VERBOSE_MAKEFILE OFF)
SET(CMAKE_TIMED_MAKEFILE   0)

option(EAC                "Use Edit and Continue mode"             OFF)
option(WFL                "Use FastLink For Debug"                 OFF)
option(PCH                "Use precompiled headers"                ON)
option(MEMCHECK           "Use Internal Memory checker"            OFF)
OPTION(DOWNLOAD_BOOST     "Download boost from sourceforge." 	   OFF)
option(INSTEX             "Detect highest instruction Extension"   OFF)
option(DYNLIB             "Optimize for Dynamic Libraries"         OFF)

option(LibMetaRuntimeSharedLinkage "Dynamic Library" ON)

IF (LibMetaRuntimeSharedLinkage)
    ADD_DEFINITIONS(-DTC_DYNAMIC_METARUNTIME=1)
ENDIF()

ADD_DEFINITIONS(-DTC_DYNAMIC_XAMLCPP=1)
ADD_DEFINITIONS(-DTC_DYNAMIC_XUI=1)
ADD_DEFINITIONS(-DTC_DYNAMIC_CLIENTDB=1)
ADD_DEFINITIONS(-DTC_DYNAMIC_CFRAMEWORK=1)
ADD_DEFINITIONS(-DTC_DYNAMIC_WMDL=1)

IF (EAC)
    MESSAGE(${ASTATUS} "[AUTO CONFIG] Disable FastLink For Debug")
    SET (WFL 0)
    MESSAGE(${ASTATUS} "[AUTO CONFIG] Activate Dynamic Linkage (for fast code apply)")
    SET (DYNLIB 1)
ENDIF(EAC)

message("Loaded Config")
