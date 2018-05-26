FUNCTION(make_qt_app project_name)

    FIND_PACKAGE(Qt REQUIRED)
    
    SET(QT_USE_QTOPENGL 1) # don't forget this
    SET(QT_USE_QTSQL 1)
    INCLUDE(${QT_USE_FILE})

    FILE(GLOB ${project_name}_source *.cpp)
    FILE(GLOB ${project_name}_header *.h)
    FILE(GLOB ${project_name}_ui *.ui)
    FILE(GLOB ${project_name}_resource *.qrc)

    QT4_WRAP_UI(${project_name}_ui_source ${${project_name}_ui})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}) #let cmake find generated ui source
    QT4_WRAP_CPP(${project_name}_moc_source ${${project_name}_header})
    QT4_ADD_RESOURCES(${project_name}_resource_source ${${project_name}_resource})

    SET(${project_name}_qt_generated ${${project_name}_ui_source} ${${project_name}_moc_source} ${${project_name}_resource_source})

    SOURCE_GROUP("Qt Generated" FILES ${${project_name}_qt_generated})

    ADD_EXECUTABLE(${project_name} ${${project_name}_source} ${${project_name}_header} ${${project_name}_qt_generated})

    TARGET_LINK_LIBRARIES(${project_name} ${QT_LIBRARIES})

ENDFUNCTION(make_qt_app)

FUNCTION(qt_support qt_files qt_libraries)

    SET(project_name a)

    FIND_PACKAGE(Qt REQUIRED)
    
    SET(QT_USE_QTOPENGL 1) # don't forget this
    SET(QT_USE_QTSQL 1)
    INCLUDE(${QT_USE_FILE})

    FILE(GLOB ${project_name}_source *.cpp)
    FILE(GLOB ${project_name}_header *.h)
    FILE(GLOB ${project_name}_ui *.ui)
    FILE(GLOB ${project_name}_resource *.qrc)

    QT4_WRAP_UI(${project_name}_ui_source ${${project_name}_ui})
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}) #let cmake find generated ui source
    QT4_WRAP_CPP(${project_name}_moc_source ${${project_name}_header})
    QT4_ADD_RESOURCES(${project_name}_resource_source ${${project_name}_resource})

    SET(${project_name}_qt_generated ${${project_name}_ui_source} ${${project_name}_moc_source} ${${project_name}_resource_source})

    SOURCE_GROUP("Qt Generated" FILES ${${project_name}_qt_generated})

    SET(${qt_files} ${${project_name}_source} ${${project_name}_header} ${${project_name}_qt_generated})

    SET(${qt_libraries} ${QT_LIBRARIES})

ENDFUNCTION(qt_support)
