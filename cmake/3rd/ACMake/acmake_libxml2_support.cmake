include(acmake_common)
include(acmake_iconv_support)

# for windows platform, download libxml2 from
# <http://www.zlatkovic.com/libxml.en.html>, and set environment variable
# `LIBXML2_HOME` to the extracted folder.
#
# usage: acmake_libxml2_support(<target name>)
macro(acmake_libxml2_support TARGET)
    acmake_iconv_support(${TARGET})

    file(TO_CMAKE_PATH $ENV{LIBXML2_HOME} ENV_LIBXML2_HOME)
    if(ENV_LIBXML2_HOME)
        list(APPEND CMAKE_INCLUDE_PATH "${ENV_LIBXML2_HOME}/include")
        list(APPEND CMAKE_LIBRARY_PATH "${ENV_LIBXML2_HOME}/lib")
    endif()
    find_package(LibXml2)
    if(NOT LIBXML2_FOUND)
        if(ENV_LIBXML2_HOME)
            message(FATAL_ERROR "Cannot find libxml2, environment variable LIBXML2_HOME may not been set correctly.")
        else()
            message(FATAL_ERROR "Cannot find libxml2, youe may need to set environment variable LIBXML2_HOME first.")
        endif()
    endif()
    include_directories(${LIBXML2_INCLUDE_DIR})
    target_link_libraries(${TARGET} ${LIBXML2_LIBRARIES})
    add_definitions(${LIBXML2_DEFINITIONS})
endmacro()
