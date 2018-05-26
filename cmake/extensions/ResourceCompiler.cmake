include(CMakeParseArguments)

function(resource_prebuild)
    set(ONE_VALUE_ARGS
        TARGET
        GENERATED_DIR
        SOURCE_ROOT
        OUT_MODULE_SOURCE
        OUT_GENERATED_FILES
        OUT_SRC
    )

    set(MULTI_VALUE_ARGS 
        RESOURCE_FILES
    )

    cmake_parse_arguments(PREBUILD_RS "" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    # source file for this reflection module
    set(MODULE_SOURCE "${PREBUILD_RS_GENERATED_DIR}/Resource_Modul_${PREBUILD_RS_TARGET}.cpp")
    
    # output the source file
    set(${PREBUILD_RS_OUT_MODULE_SOURCE} ${MODULE_SOURCE} PARENT_SCOPE)

    set(GENERATED_FILES "${MODULE_SOURCE}")

    # output it to the output source files
    set(${PREBUILD_RS_OUT_SRC} ${${PREBUILD_RS_OUT_SRC}} "${MODULE_SOURCE}" PARENT_SCOPE)

    foreach (HEADER ${PREBUILD_RS_RESOURCE_FILES})
        get_filename_component(DIRECTORY_NAME ${HEADER} DIRECTORY)
        get_filename_component(BASE_NAME ${HEADER} NAME_WE)
        get_filename_component(EXTENSION ${HEADER} EXT)

        # make sure the path is absolute
        if (NOT IS_ABSOLUTE ${HEADER})
            set(DIRECTORY_NAME "${PREBUILD_RS_SOURCE_ROOT}/${DIRECTORY_NAME}")
        endif ()

        # skip hpp files
        if (NOT "${EXTENSION}" STREQUAL ".hpp")
            file(RELATIVE_PATH RELATIVE 
                ${PREBUILD_RS_SOURCE_ROOT} 
                "${DIRECTORY_NAME}/Resource_${BASE_NAME}"
            )

            set(GENERATED_SOURCE "${PREBUILD_RS_GENERATED_DIR}/${RELATIVE}.cpp")

            list(APPEND GENERATED_FILES ${GENERATED_SOURCE})
        endif ()
    endforeach ()

    source_group(".Resource" FILES ${GENERATED_FILES})

    set(${PREBUILD_RS_OUT_GENERATED_FILES} "${GENERATED_FILES}" PARENT_SCOPE)
endfunction ()

function(resource_build)
    set(ONE_VALUE_ARGS
        TARGET
        SOURCE_ROOT
        SOURCE_FILE
        MODULE_SOURCE_FILE
        GENERATED_DIR
        RESOURCE_EXECUTABLE
    )

    set(MULTI_VALUE_ARGS 
        GENERATED_FILES
        RESOURCE_FILES
    )

    cmake_parse_arguments(BUILD_RS "" "${ONE_VALUE_ARGS}" "${MULTI_VALUE_ARGS}" ${ARGN})

    get_property(DIRECTORIES TARGET ${BUILD_RS_TARGET} PROPERTY INCLUDE_DIRECTORIES)

    set(INCLUDES "")

    foreach (INC ${BUILD_RS_RESOURCE_FILES})
        set(INCLUDES "${INCLUDES}${INC}\n")
    endforeach ()

    set(RAW_INCLUDES ${GLOBAL_META_INCLUDES} ${DIRECTORIES} ${BUILD_RS_INCLUDES})
    
    file(WRITE ${BUILD_RS_SOURCE_FILE} ${INCLUDES})

    foreach (GENERATED_FILE ${BUILD_RS_GENERATED_FILES})
        get_filename_component(EXTENSION ${GENERATED_FILE} EXT)

        set_source_files_properties(${GENERATED_FILE} PROPERTIES GENERATED TRUE)

        # we have to create the files, as they might not be written to
        if (NOT EXISTS ${GENERATED_FILE})
            file(WRITE ${GENERATED_FILE} ${EMPTY_SOURCE_CONTENTS})
        endif ()
    endforeach ()

    get_filename_component(WORK_DIRECTORY_EXEC ${BUILD_RS_RESOURCE_EXECUTABLE} DIRECTORY)
    get_filename_component(EXEC_NAME ${BUILD_RS_RESOURCE_EXECUTABLE} NAME)

    #message("
    #    OUTPUT ${BUILD_RS_GENERATED_FILES}
    #    DEPENDS ${BUILD_RS_RESOURCE_FILES}
    #    WORKING_DIRECTORY ${WORK_DIRECTORY_EXEC}
    #    COMMAND 
    #    ${EXEC_NAME}
    #    --target-name "${BUILD_RS_TARGET}"
    #    --source-root "${BUILD_RS_SOURCE_ROOT}"
    #    --in-source "${BUILD_RS_SOURCE_FILE}"
    #    --out-source "${BUILD_RS_MODULE_SOURCE_FILE}"
    #    --out-dir "${BUILD_RS_GENERATED_DIR}"")

    # add the command that generates the header and source files
    add_custom_command(
        OUTPUT ${BUILD_RS_GENERATED_FILES}
        DEPENDS ${BUILD_RS_RESOURCE_FILES}
        WORKING_DIRECTORY ${WORK_DIRECTORY_EXEC}
        COMMAND 
        ${EXEC_NAME}
        --target-name "${BUILD_RS_TARGET}"
        --source-root "${BUILD_RS_SOURCE_ROOT}"
        --in-source "${BUILD_RS_SOURCE_FILE}"
        --out-source "${BUILD_RS_MODULE_SOURCE_FILE}"
        --out-dir "${BUILD_RS_GENERATED_DIR}"
        COMMENT "[RS]: Generate resource"
    )
endfunction ()