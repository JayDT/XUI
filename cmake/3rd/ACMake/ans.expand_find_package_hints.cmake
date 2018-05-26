include(ans.env)

macro(expand_find_package_hints NAME)
    set(${NAME}_FIND_PACKAGE_HINTS ${ACMAKE_FIND_PACKAGE_HINTS})
    string(TOLOWER ${NAME} EXPAND_FIND_PACKAGE_HINTS_LOWER)
    string(TOUPPER ${NAME} EXPAND_FIND_PACKAGE_HINTS_UPPER)
    foreach(D ${ACMAKE_FIND_PACKAGE_HINTS})
        list(APPEND ${NAME}_FIND_PACKAGE_HINTS "${D}/${NAME}")
        list(APPEND ${NAME}_FIND_PACKAGE_HINTS
            "${D}/${EXPAND_FIND_PACKAGE_HINTS_LOWER}")
        list(APPEND ${NAME}_FIND_PACKAGE_HINTS
            "${D}/${EXPAND_FIND_PACKAGE_HINTS_UPPER}")
    endforeach()
endmacro()
