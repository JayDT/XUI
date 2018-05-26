# 从files中移除'../src/main.cpp', 用于单元测试.
macro(remove_main_from_test deps files)
    get_filename_component(parent_dir ${CMAKE_CURRENT_SOURCE_DIR} PATH)
    string(REGEX REPLACE "${parent_dir}/src/main.cpp" "" ${deps} "${files}")
endmacro()
