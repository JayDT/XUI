# warning: don't use this, use acmake_qt_support instead

include(ans.parse_arguments)

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

MACRO(qt_support)
    FIND_PACKAGE(Qt REQUIRED)
    SET(QT_USE_QTOPENGL 1) # don't forget this
    SET(QT_USE_QTSQL 1)
    INCLUDE(${QT_USE_FILE})

    if(${ARGC} GREATER 0)
        PARSE_ARGUMENTS(
            QT_SUPPORT
            "HEADER;UI;RESOURSE"
            ""
            ${ARGN}
            )
        CAR(qt_files ${QT_SUPPORT_DEFAULT_ARGS})
        CDR(QT_SUPPORT_REST ${QT_SUPPORT_DEFAULT_ARGS})
        CAR(qt_libraries ${QT_SUPPORT_REST})

        SET(project_name ${PROJECT_NAME})

        #FILE(GLOB_RECURSE ${project_name}_source *.cpp)
        FILE(GLOB_RECURSE ${project_name}_header *.h *.hpp)
        FILE(GLOB_RECURSE ${project_name}_ui *.ui)
        FILE(GLOB_RECURSE ${project_name}_resource *.qrc)
        list(APPEND ${project_name}_header ${QT_SUPPORT_HEADER})
        list(APPEND ${project_name}_ui ${QT_SUPPORT_UI})
        list(APPEND ${project_name}_resource ${QT_SUPPORT_RESOURCE})

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
    endif()
ENDMACRO(qt_support)
