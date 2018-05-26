include(acmake_common)

# usage: acmake_mysql_support(<target>)
macro(acmake_mysql_support TARGET)
    find_library(MySql REQUIRED)
    include_directories(${MYSQL_INCLUDE_DIRS})
    target_link_libraries(${TARGET} ${MYSQL_LIBRARIES})
endmacro()
