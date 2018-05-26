include(acmake_common)

macro(acmake_mysqlconn_support TARGET)
    find_package(MySqlConn REQUIRED)
    include_directories(${MYSQLCONN_INCLUDE_DIRS})
    target_link_libraries(${TARGET} ${MYSQLCONN_LIBRARIES})
endmacro()
