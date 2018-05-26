# get environment variable ${PREFIX}_HOME or ${PREFIX}_ROOT
macro(acmake_env_home OUTVAR PREFIX)
    if(NOT "$ENV{${PREFIX}_HOME}" STREQUAL "")
        file(TO_CMAKE_PATH "$ENV{${PREFIX}_HOME}" ${OUTVAR})
    endif()
    if(NOT "$ENV{${PREFIX}_ROOT}" STREQUAL "")
        file(TO_CMAKE_PATH "$ENV{${PREFIX}_ROOT}" ${OUTVAR})
    endif()
endmacro()
