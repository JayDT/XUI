macro(filter_files_contain pattern out_var in_var)
    set(${out_var}_temp)
    foreach(file_name ${${in_var}})
        set(z_filter_file_contain_content)
        file(READ ${file_name} z_filter_file_contain_content)
        if(z_filter_file_contain_content MATCHES ${pattern})
            list(APPEND ${out_var}_temp ${file_name})
        endif(z_filter_file_contain_content MATCHES ${pattern})
    endforeach(file_name)
    set(${out_var} ${${out_var}_temp})
endmacro()

MACRO(qt_support qt_files qt_libraries)

    SET(project_name ${PROJECT_NAME})

    FIND_PACKAGE(Qt REQUIRED)
    
    SET(QT_USE_QTOPENGL 1) # don't forget this
    SET(QT_USE_QTSQL 1)
    INCLUDE(${QT_USE_FILE})

    #FILE(GLOB_RECURSE ${project_name}_source *.cpp)
    FILE(GLOB_RECURSE ${project_name}_header *.h *.hpp)
    FILE(GLOB_RECURSE ${project_name}_ui *.ui)
    FILE(GLOB_RECURSE ${project_name}_resource *.qrc)

    filter_files_contain(".*Q_OBJECT.*" ${project_name}_header ${project_name}_header)

    QT4_WRAP_UI(${project_name}_ui_source ${${project_name}_ui})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}) #let cmake find generated ui source
    QT4_WRAP_CPP(${project_name}_moc_source ${${project_name}_header})
    QT4_ADD_RESOURCES(${project_name}_resource_source ${${project_name}_resource})

    SET(${project_name}_qt_generated ${${project_name}_ui_source} ${${project_name}_moc_source} ${${project_name}_resource_source})

    SOURCE_GROUP("Qt Generated" FILES ${${project_name}_qt_generated})

    #SET(${qt_files} ${${project_name}_source} ${${project_name}_header} ${${project_name}_qt_generated})
    SET(${qt_files} ${${project_name}_qt_generated})

    SET(${qt_libraries} ${QT_LIBRARIES})

ENDMACRO(qt_support)
