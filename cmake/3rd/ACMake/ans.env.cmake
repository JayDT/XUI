if(NOT ANS_ENV_INCLUDED)
    set(ANS_TEMPLATE_DIR "${ACMake_INSTALL_DIR}/template")
    set(ANS_ENV_INCLUDED TRUE)
    set(CMLEX_FIND_PACKAGE_HINTS ${CMAKE_SOURCE_DIR}/misc/3rd)

    # default sdk path
    if(WIN32 AND NOT ACMAKE_SDK_PATH)
        file(TO_CMAKE_PATH $ENV{PROGRAMFILES} ACMAKE_SDK_PATH)
        set(ACMAKE_SDK_PATH "${ACMAKE_SDK_PATH}/sdk")
    endif()

    # find package hints
    set(ACMAKE_FIND_PACKAGE_HINTS ${CMAKE_SOURCE_DIR}/misc/3rd)
endif()
