# include directories
find_path(CURL_INCLUDE_DIRS
    NAMES "curl.h"
    PATHS
        "${dep}/libcurl/"
)

# shared library directory
find_library(CURL_LIBRARY
    NAMES "libcurl"
    PATHS
        "${dep}/lib/${PLATFORM_NAME}_${BRT_NAME}/"
)

message(${ASTATUS} "Found CURL headers       : ${CURL_INCLUDE_DIRS}")
message(${ASTATUS} "Found CURL library       : ${CURL_LIBRARY}")

# unable to find everything
if (NOT CURL_INCLUDE_DIRS OR
    NOT CURL_LIBRARY)
    message(FATAL_ERROR 
        "Unable to find CURL installation. "
        "Make sure that \"CURL_ROOT\" is set with the installation directory in either an environment variable or through the CMake GUI."
    )
endif ()