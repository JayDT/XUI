# don't use this, use `acmake_boost_support` instead

include(ans.env)
include(ans.parse_arguments)


# Include [and link] with boost.
# 
# Use shared and multithread version
# Decltype will be supported under MSVC10.
# USAGE: boost_support([<target>])
# 
# @param target libs will be linked if it is specified
macro(boost_support)
    parse_arguments(
        BOOST_SUPPORT
        "COMPONENTS"
        ""
        ${ARGN}
        )
    car(BOOST_SUPPORT_TARGET ${BOOST_SUPPORT_DEFAULT_ARGS})
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_MULTITHREADED ON)
    if(BOOST_SUPPORT_COMPONENTS)
        if (NOT BOOST_SUPPORT_TARGET)
            message(FATAL_ERROR "Target must be specified.")
        endif()
        find_package(Boost COMPONENTS
            ${BOOST_SUPPORT_COMPONENTS}
            #date_time
            #filesystem
            #graph
            #iostreams
            #regex
            #serialization
            #signals
            #system
            #thread
            #unit_test_framework
            #wave
            )
        link_directories(${Boost_LIBRARY_DIRS})
        target_link_libraries("${BOOST_SUPPORT_TARGET}" ${Boost_LIBRARIES})

        # use shared library in unit test {{{
        list(FIND BOOST_SUPPORT_COMPONENTS unit_test_framework
            BOOST_SUPPORT_UNIT_TEST)
        if(NOT "${BOOST_SUPPORT_UNIT_TEST}" EQUAL "-1")
            add_definitions(-DBOOST_TEST_DYN_LINK)
        endif()
        # }}}
    else()
        # head only support
        find_package(Boost REQUIRED)
    endif()

    boost_config()
    include_directories(${Boost_INCLUDE_DIRS})

    # try to find extensions
    find_package(OvenToBoost)
    if(OVEN_TO_BOOST_FOUND)
        #message("${OVEN_TO_BOOST_INCLUDE_DIRS}")
        include_directories(${OVEN_TO_BOOST_INCLUDE_DIRS})
    endif()
endmacro()

# include boost header files, also extensions
# decltype will be supported under MSVC10
macro(boost_header_only_support)
    if(NOT ANS_BOOST_HEADER_ONLY_SUPPORTED)
        set(Boost_USE_STATIC_LIBS OFF)
        set(Boost_USE_MULTITHREADED ON)
        find_package(Boost REQUIRED)
        include_directories(${Boost_INCLUDE_DIRS})
        boost_config()
        # try to find extensions
        find_package(OvenToBoost)
        if(OVEN_TO_BOOST_FOUND)
            #message("${OVEN_TO_BOOST_INCLUDE_DIRS}")
            include_directories(${OVEN_TO_BOOST_INCLUDE_DIRS})
        endif()
        set(ANS_BOOST_HEADER_ONLY_SUPPORTED TRUE)
    endif()
endmacro()

# helper function
# decltype will be supported under MSVC10
macro(boost_config)
    # decltype
    if(MSVC10)
        add_definitions(-DBOOST_RESULT_OF_USE_DECLTYPE)
    endif()
    # disable auto link on windows, use dynamic boost lib instead
    add_definitions(-DBOOST_ALL_NO_LIB)
    add_definitions(-DBOOST_LIB_DIAGNOSTIC)
endmacro()
