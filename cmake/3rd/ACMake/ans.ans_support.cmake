include(acmake_common)

macro(ans_support)
    find_package(Ans REQUIRED)
    include_directories(${ANS_INCLUDE_DIRS})
endmacro()
