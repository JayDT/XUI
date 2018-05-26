include(acmake_common)
include(acmake_simple_support)

# usage: acmake_cppunit_support(<target>)
macro(acmake_cppunit_support TARGET)
    acmake_simple_support(${TARGET} CppUnit)
endmacro()
