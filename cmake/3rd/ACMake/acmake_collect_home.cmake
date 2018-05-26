include(acmake_env_home)

# collect home path to ${PREFIX}_HOME
# order:
#   ${PREFIX}_HOME
#   ${PREFIX}_ROOT
#   $ENV{${PREFIX}_HOME} (path will be converted to cmake format)
#   $ENV{${PREFIX}_ROOT} (path will be converted to cmake format)
macro(acmake_collect_home PREFIX)
    if(NOT ${PREFIX}_HOME)
        if(${PREFIX}_ROOT)
            set(${PREFIX}_HOME "${${PREFIX}_ROOT}")
        else()
            acmake_env_home(${PREFIX}_HOME ${PREFIX})
        endif()
    endif()
endmacro()
