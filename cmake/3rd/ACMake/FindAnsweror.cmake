# ANSWEROR_FOUND
# ANSWEROR_INCLUDE_DIRS
# ANSWEROR_LIBRARIES
# answeror at 2011-11-06

if(NOT ANSWEROR_FOUND)
    include(FindPkgMacros)
    findpkg_begin(ANSWEROR)

    find_package(Ans)
    find_package(DynamicAny)
    find_package(OvenToBoost)

    if(ANS_FOUND AND DYNAMIC_ANY_FOUND AND OVEN_TO_BOOST_FOUND)
        set(ANSWEROR_INCLUDE_DIR
            ${ANS_INCLUDE_DIRS}
            ${DYNAMIC_ANY_INCLUDE_DIRS}
            ${OVEN_TO_BOOST_INCLUDE_DIRS}
            )
        set(ANSWEROR_LIBRARIES
            ${ANS_LIBRARIES}
            ${DYNAMIC_ANY_LIBRARIES}
            )
    endif()

    findpkg_finish(ANSWEROR)
endif()
