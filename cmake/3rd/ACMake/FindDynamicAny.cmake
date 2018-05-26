# DYNAMIC_ANY_FOUND
# DYNAMIC_ANY_INCLUDE_DIRS
# answeror at 2011-11-05

include(FindPkgMacros)
findpkg_begin(DYNAMIC_ANY)

# get home path from enviounment
getenv_path(DYNAMIC_ANY_HOME)
if(NOT ENV_DYNAMIC_ANY_HOME)
    getenv_path(DYNAMIC_ANY_ROOT)
    set(ENV_DYNAMIC_ANY_HOME ${ENV_DYNAMIC_ANY_ROOT})
    if(NOT ENV_DYNAMIC_ANY_HOME)
        getenv_path(CODE)
        if(ENV_CODE)
            set(MAYBE ${ENV_CODE}/others/Boost.DynamicAny)
            if(EXISTS ${MAYBE})
                set(ENV_DYNAMIC_ANY_HOME ${MAYBE})
            endif()
        endif()
    endif()
endif()

if(ENV_DYNAMIC_ANY_HOME)
    set(DYNAMIC_ANY_INCLUDE_DIR ${ENV_DYNAMIC_ANY_HOME}/include)
endif()

findpkg_finish(DYNAMIC_ANY)
