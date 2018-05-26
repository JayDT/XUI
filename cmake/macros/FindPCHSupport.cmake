FUNCTION(GET_COMMON_PCH_PARAMS PCH_HEADER PCH_FE INCLUDE_PREFIX)
  GET_FILENAME_COMPONENT(PCH_HEADER_N ${PCH_HEADER} NAME)
  GET_DIRECTORY_PROPERTY(TARGET_INCLUDES INCLUDE_DIRECTORIES)

  FOREACH(ITEM ${TARGET_INCLUDES})
    LIST(APPEND INCLUDE_FLAGS_LIST "${INCLUDE_PREFIX}\"${ITEM}\" ")
  ENDFOREACH(ITEM)

  SET(PCH_HEADER_NAME ${PCH_HEADER_N} PARENT_SCOPE)
  SET(PCH_HEADER_OUT ${CMAKE_CURRENT_BINARY_DIR}/${PCH_HEADER_N}.${PCH_FE} PARENT_SCOPE)
  SET(INCLUDE_FLAGS ${INCLUDE_FLAGS_LIST} PARENT_SCOPE)
ENDFUNCTION(GET_COMMON_PCH_PARAMS)

FUNCTION(GENERATE_CXX_PCH_COMMAND TARGET_NAME INCLUDE_FLAGS IN PCH_SRC OUT)
  IF (CMAKE_BUILD_TYPE)
    STRING(TOUPPER _${CMAKE_BUILD_TYPE} CURRENT_BUILD_TYPE)
  ENDIF ()

  SET(COMPILE_FLAGS ${CMAKE_CXX_FLAGS${CURRENT_BUILD_TYPE}})
  LIST(APPEND COMPILE_FLAGS ${CMAKE_CXX_FLAGS})

  IF ("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin")
    IF (NOT "${CMAKE_OSX_ARCHITECTURES}" STREQUAL "")
      LIST(APPEND COMPILE_FLAGS "-arch ${CMAKE_OSX_ARCHITECTURES}")
    ENDIF ()
    IF (NOT "${CMAKE_OSX_SYSROOT}" STREQUAL "")
      LIST(APPEND COMPILE_FLAGS "-isysroot ${CMAKE_OSX_SYSROOT}")
    ENDIF ()
    IF (NOT "${CMAKE_OSX_DEPLOYMENT_TARGET}" STREQUAL "")
      LIST(APPEND COMPILE_FLAGS "-mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}")
    ENDIF ()
  ENDIF ()

  GET_DIRECTORY_PROPERTY(TARGET_DEFINITIONS COMPILE_DEFINITIONS)
  FOREACH(ITEM ${TARGET_DEFINITIONS})
    string(REPLACE "\"" "\\\"" ITEM ${ITEM})
    LIST(APPEND DEFINITION_FLAGS "-D${ITEM}")
  ENDFOREACH(ITEM)

  SEPARATE_ARGUMENTS(COMPILE_FLAGS)
  SEPARATE_ARGUMENTS(INCLUDE_FLAGS)
  SEPARATE_ARGUMENTS(DEFINITION_FLAGS)

  GET_FILENAME_COMPONENT(PCH_SRC_N ${PCH_SRC} NAME)
  ADD_LIBRARY(${PCH_SRC_N}_${TARGET_NAME}_dephelp MODULE ${PCH_SRC})

  #message("------ PCH: ${DEFINITION_FLAGS}")

  ADD_CUSTOM_COMMAND(
    OUTPUT ${OUT}
    COMMAND ${CMAKE_CXX_COMPILER}
    ARGS ${DEFINITION_FLAGS} ${COMPILE_FLAGS} ${INCLUDE_FLAGS} -x c++-header ${IN} -o ${OUT}
    DEPENDS ${IN} ${PCH_SRC_N}_${TARGET_NAME}_dephelp
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

  ADD_CUSTOM_TARGET(generate_${PCH_SRC_N}_${TARGET_NAME}
    DEPENDS ${OUT}
  )

  ADD_DEPENDENCIES(${TARGET_NAME} generate_${PCH_SRC_N}_${TARGET_NAME})
ENDFUNCTION(GENERATE_CXX_PCH_COMMAND)

FUNCTION(ADD_CXX_PCH_GCC TARGET_NAME PCH_HEADER PCH_SOURCE)
  GET_COMMON_PCH_PARAMS(${PCH_HEADER} "gch" "-I")
  GENERATE_CXX_PCH_COMMAND(${TARGET_NAME} "${INCLUDE_FLAGS}" ${PCH_HEADER} ${PCH_SOURCE} ${PCH_HEADER_OUT})
  SET_TARGET_PROPERTIES(
    ${TARGET_NAME} PROPERTIES
    COMPILE_FLAGS "-include ${CMAKE_CURRENT_BINARY_DIR}/${PCH_HEADER_NAME}"
  )
ENDFUNCTION(ADD_CXX_PCH_GCC)

FUNCTION(ADD_CXX_PCH_CLANG TARGET_NAME PCH_HEADER PCH_SOURCE)
  GET_COMMON_PCH_PARAMS(${PCH_HEADER} "pch" "-I")
  GENERATE_CXX_PCH_COMMAND(${TARGET_NAME} "${INCLUDE_FLAGS}" ${PCH_HEADER} ${PCH_SOURCE} ${PCH_HEADER_OUT})
  SET_TARGET_PROPERTIES(
    ${TARGET_NAME} PROPERTIES
    COMPILE_FLAGS "-include-pch ${PCH_HEADER_OUT}"
  )
ENDFUNCTION(ADD_CXX_PCH_CLANG)

FUNCTION(ADD_CXX_PCH_MSVC TARGET_NAME PCH_HEADER PCH_SOURCE)
  GET_COMMON_PCH_PARAMS(${PCH_HEADER} "pch" "/I")
  SET_TARGET_PROPERTIES(
    ${TARGET_NAME} PROPERTIES
    COMPILE_FLAGS "/FI${PCH_HEADER_NAME} /Yu${PCH_HEADER_NAME}"
  )
  SET_SOURCE_FILES_PROPERTIES(
    ${PCH_SOURCE} PROPERTIES
    COMPILE_FLAGS "/Yc${PCH_HEADER_NAME}"
  )
ENDFUNCTION(ADD_CXX_PCH_MSVC)

FUNCTION(ADD_CXX_PCH TARGET_NAME PCH_HEADER PCH_SOURCE)
  IF (MSVC)
    ADD_CXX_PCH_MSVC(${TARGET_NAME} ${PCH_HEADER} ${PCH_SOURCE})
  ELSEIF ("${CMAKE_GENERATOR}" MATCHES "Xcode")
    SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
      XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER YES
      XCODE_ATTRIBUTE_GCC_PREFIX_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/${PCH_HEADER}"
    )
  ELSEIF ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    ADD_CXX_PCH_CLANG(${TARGET_NAME} ${PCH_HEADER} ${PCH_SOURCE})
  ELSEIF ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    ADD_CXX_PCH_GCC(${TARGET_NAME} ${PCH_HEADER} ${PCH_SOURCE})
  ENDIF ()
ENDFUNCTION(ADD_CXX_PCH)
# - Try to find precompiled headers support for GCC 3.4 and 4.x (and MSVC)
# Once done this will define:
#
# Variable:
#   PCHSupport_FOUND
#
# Macro:
#   ADD_PRECOMPILED_HEADER  _targetName _input  _dowarn
#   ADD_PRECOMPILED_HEADER_TO_TARGET _targetName _input _pch_output_to_use _dowarn
#   ADD_NATIVE_PRECOMPILED_HEADER _targetName _input _dowarn
#   GET_NATIVE_PRECOMPILED_HEADER _targetName _input

IF(CMAKE_COMPILER_IS_GNUCXX)

    EXEC_PROGRAM(
      ${CMAKE_CXX_COMPILER}
      ARGS     ${CMAKE_CXX_COMPILER_ARG1} -dumpversion
      OUTPUT_VARIABLE gcc_compiler_version
    )
    #MESSAGE("GCC Version: ${gcc_compiler_version}")
    IF(gcc_compiler_version MATCHES "4\\.[0-9]\\.[0-9]")
      SET(PCHSupport_FOUND TRUE)
    ELSE(gcc_compiler_version MATCHES "4\\.[0-9]\\.[0-9]")
      IF(gcc_compiler_version MATCHES "3\\.4\\.[0-9]")
        SET(PCHSupport_FOUND TRUE)
      ENDIF(gcc_compiler_version MATCHES "3\\.4\\.[0-9]")
    ENDIF(gcc_compiler_version MATCHES "4\\.[0-9]\\.[0-9]")

    SET(_PCH_include_prefix "-I")

ELSE(CMAKE_COMPILER_IS_GNUCXX)

    IF(WIN32)
      SET(PCHSupport_FOUND TRUE) # for experimental msvc support
      SET(_PCH_include_prefix "/I")
    ELSE(WIN32)
      SET(PCHSupport_FOUND FALSE)
    ENDIF(WIN32)

ENDIF(CMAKE_COMPILER_IS_GNUCXX)

MACRO(_PCH_GET_COMPILE_FLAGS _out_compile_flags)

  STRING(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _flags_var_name)
  SET(${_out_compile_flags} ${${_flags_var_name}} )

  IF(CMAKE_COMPILER_IS_GNUCXX)
    GET_TARGET_PROPERTY(_targetType ${_PCH_current_target} TYPE)
    IF(${_targetType} STREQUAL SHARED_LIBRARY AND NOT WIN32)
      LIST(APPEND ${_out_compile_flags} -fPIC)
    ENDIF()

  ELSE(CMAKE_COMPILER_IS_GNUCXX)
    ## TODO ... ? or does it work out of the box
  ENDIF(CMAKE_COMPILER_IS_GNUCXX)

  GET_DIRECTORY_PROPERTY(DIRINC INCLUDE_DIRECTORIES )
  FOREACH(item ${DIRINC})
    LIST(APPEND ${_out_compile_flags} "${_PCH_include_prefix}${item}")
  ENDFOREACH(item)

  GET_DIRECTORY_PROPERTY(_directory_flags DEFINITIONS)
  GET_DIRECTORY_PROPERTY(_global_definitions DIRECTORY ${CMAKE_SOURCE_DIR} DEFINITIONS)
  #MESSAGE("_directory_flags ${_directory_flags} ${_global_definitions}" )
  LIST(APPEND ${_out_compile_flags} ${_directory_flags})
  LIST(APPEND ${_out_compile_flags} ${_global_definitions})
  LIST(APPEND ${_out_compile_flags} ${CMAKE_CXX_FLAGS} )

  SEPARATE_ARGUMENTS(${_out_compile_flags})

ENDMACRO(_PCH_GET_COMPILE_FLAGS)

MACRO(_PCH_WRITE_PCHDEP_CXX _targetName _include_file _dephelp)

  SET(${_dephelp} ${CMAKE_CURRENT_BINARY_DIR}/${_targetName}_pch_dephelp.cxx)
  FILE(WRITE  ${${_dephelp}}
"#include \"${_include_file}\"
int testfunction()
{
    return 0;
}
"
  )

ENDMACRO(_PCH_WRITE_PCHDEP_CXX )

MACRO(_PCH_GET_COMPILE_COMMAND out_command _input _output)

    FILE(TO_NATIVE_PATH ${_input} _native_input)
    FILE(TO_NATIVE_PATH ${_output} _native_output)


    IF(CMAKE_COMPILER_IS_GNUCXX)
      IF(CMAKE_CXX_COMPILER_ARG1)
    # remove leading space in compiler argument
        STRING(REGEX REPLACE "^ +" "" pchsupport_compiler_cxx_arg1 ${CMAKE_CXX_COMPILER_ARG1})

    SET(${out_command}
      ${CMAKE_CXX_COMPILER} ${pchsupport_compiler_cxx_arg1} ${_compile_FLAGS}    -x c++-header -o ${_output} ${_input}
      )
      ELSE(CMAKE_CXX_COMPILER_ARG1)
    SET(${out_command}
      ${CMAKE_CXX_COMPILER}  ${_compile_FLAGS}    -x c++-header -o ${_output} ${_input}
      )
      ENDIF(CMAKE_CXX_COMPILER_ARG1)
    ELSE(CMAKE_COMPILER_IS_GNUCXX)

    SET(_dummy_str "#include <${_input}>")
    FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/pch_dummy.cpp ${_dummy_str})

    SET(${out_command}
        ${CMAKE_CXX_COMPILER} ${_compile_FLAGS} /c /Fp${_native_output} /Yc${_native_input} pch_dummy.cpp
    )
    #/out:${_output}

    ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ENDMACRO(_PCH_GET_COMPILE_COMMAND )



MACRO(_PCH_GET_TARGET_COMPILE_FLAGS _cflags  _header_name _pch_path _dowarn )

  FILE(TO_NATIVE_PATH ${_pch_path} _native_pch_path)

  IF(CMAKE_COMPILER_IS_GNUCXX)
    # for use with distcc and gcc >4.0.1 if preprocessed files are accessible
    # on all remote machines set
    # PCH_ADDITIONAL_COMPILER_FLAGS to -fpch-preprocess
    # if you want warnings for invalid header files (which is very inconvenient
    # if you have different versions of the headers for different build types
    # you may set _pch_dowarn
    IF (_dowarn)
      SET(${_cflags} "${PCH_ADDITIONAL_COMPILER_FLAGS} -include ${CMAKE_CURRENT_BINARY_DIR}/${_header_name} -Winvalid-pch " )
    ELSE (_dowarn)
      SET(${_cflags} "${PCH_ADDITIONAL_COMPILER_FLAGS} -include ${CMAKE_CURRENT_BINARY_DIR}/${_header_name} " )
    ENDIF (_dowarn)
  ELSE(CMAKE_COMPILER_IS_GNUCXX)

    set(${_cflags} "/Fp${_native_pch_path} /Yu${_header_name}" )

  ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ENDMACRO(_PCH_GET_TARGET_COMPILE_FLAGS )

MACRO(GET_PRECOMPILED_HEADER_OUTPUT _targetName _input _output)
  GET_FILENAME_COMPONENT(_name ${_input} NAME)
  GET_FILENAME_COMPONENT(_path ${_input} PATH)
  SET(_output "${CMAKE_CURRENT_BINARY_DIR}/${_name}.gch/${_targetName}_${CMAKE_BUILD_TYPE}.gch")
ENDMACRO(GET_PRECOMPILED_HEADER_OUTPUT _targetName _input)


MACRO(ADD_PRECOMPILED_HEADER_TO_TARGET _targetName _input _pch_output_to_use )

  # to do: test whether compiler flags match between target  _targetName
  # and _pch_output_to_use
  GET_FILENAME_COMPONENT(_name ${_input} NAME)

  IF( "${ARGN}" STREQUAL "0")
    SET(_dowarn 0)
  ELSE( "${ARGN}" STREQUAL "0")
    SET(_dowarn 1)
  ENDIF("${ARGN}" STREQUAL "0")


  _PCH_GET_TARGET_COMPILE_FLAGS(_target_cflags ${_name} ${_pch_output_to_use} ${_dowarn})
  MESSAGE("Add flags ${_target_cflags} to ${_targetName} " )
  SET_TARGET_PROPERTIES(${_targetName}
    PROPERTIES
    COMPILE_FLAGS ${_target_cflags}
  )

  ADD_CUSTOM_TARGET(pch_Generate_${_targetName}
    DEPENDS    ${_pch_output_to_use}
  )

  ADD_DEPENDENCIES(${_targetName} pch_Generate_${_targetName} )

ENDMACRO(ADD_PRECOMPILED_HEADER_TO_TARGET)

MACRO(ADD_PRECOMPILED_HEADER _targetName _input)

  SET(_PCH_current_target ${_targetName})

  IF(NOT CMAKE_BUILD_TYPE)
    MESSAGE(FATAL_ERROR
      "This is the ADD_PRECOMPILED_HEADER macro. "
      "You must set CMAKE_BUILD_TYPE!"
      )
  ENDIF(NOT CMAKE_BUILD_TYPE)

  IF( "${ARGN}" STREQUAL "0")
    SET(_dowarn 0)
  ELSE( "${ARGN}" STREQUAL "0")
    SET(_dowarn 1)
  ENDIF("${ARGN}" STREQUAL "0")

  GET_FILENAME_COMPONENT(_name ${_input} NAME)
  GET_FILENAME_COMPONENT(_path ${_input} PATH)
  GET_PRECOMPILED_HEADER_OUTPUT( ${_targetName} ${_input} _output)

  GET_FILENAME_COMPONENT(_outdir ${_output} PATH )

  GET_TARGET_PROPERTY(_targetType ${_PCH_current_target} TYPE)
   _PCH_WRITE_PCHDEP_CXX(${_targetName} ${_input} _pch_dephelp_cxx)

  IF(${_targetType} STREQUAL SHARED_LIBRARY)
    ADD_LIBRARY(${_targetName}_pch_dephelp STATIC ${_pch_dephelp_cxx} )
  ELSE(${_targetType} STREQUAL SHARED_LIBRARY)
    ADD_LIBRARY(${_targetName}_pch_dephelp STATIC ${_pch_dephelp_cxx})
  ENDIF(${_targetType} STREQUAL SHARED_LIBRARY)

  FILE(MAKE_DIRECTORY ${_outdir})

  _PCH_GET_COMPILE_FLAGS(_compile_FLAGS)

  #MESSAGE("_compile_FLAGS: ${_compile_FLAGS}")
  #message("COMMAND ${CMAKE_CXX_COMPILER}    ${_compile_FLAGS} -x c++-header -o ${_output} ${_input}")
  SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_BINARY_DIR}/${_name} PROPERTIES GENERATED 1)
  ADD_CUSTOM_COMMAND(
   OUTPUT    ${CMAKE_CURRENT_BINARY_DIR}/${_name}
   COMMAND ${CMAKE_COMMAND} -E copy  ${_input} ${CMAKE_CURRENT_BINARY_DIR}/${_name} # ensure same directory! Required by gcc
   DEPENDS ${_input}
  )

  #message("_command  ${_input} ${_output}")
  _PCH_GET_COMPILE_COMMAND(_command  ${CMAKE_CURRENT_BINARY_DIR}/${_name} ${_output} )

  #message(${_input} )
  #message("_output ${_output}")

  ADD_CUSTOM_COMMAND(
    OUTPUT ${_output}
    COMMAND ${_command}
    DEPENDS ${_input}   ${CMAKE_CURRENT_BINARY_DIR}/${_name} ${_targetName}_pch_dephelp
  )

  ADD_PRECOMPILED_HEADER_TO_TARGET(${_targetName} ${_input}  ${_output} ${_dowarn})
ENDMACRO(ADD_PRECOMPILED_HEADER)

# Generates the use of precompiled in a target,
# without using depency targets (2 extra for each target)
# Using Visual, must also add ${_targetName}_pch to sources
# Not needed by Xcode

MACRO(GET_NATIVE_PRECOMPILED_HEADER _targetName _input)

    if(CMAKE_GENERATOR MATCHES Visual*)

    SET(_dummy_str "#include \"${_input}\"\n"
                    "// This is required to suppress LNK4221.  Very annoying.\n"
                    "void *g_${_targetName}Dummy = 0\;\n")

    # Use of cxx extension for generated files (as Qt does)
    SET(${_targetName}_pch ${CMAKE_CURRENT_BINARY_DIR}/${_targetName}_pch.cxx)
    if(EXISTS ${${_targetName}_pch})
        # Check if contents is the same, if not rewrite
        # todo
    else(EXISTS ${${_targetName}_pch})
        FILE(WRITE ${${_targetName}_pch} ${_dummy_str})
    endif(EXISTS ${${_targetName}_pch})
    endif(CMAKE_GENERATOR MATCHES Visual*)

ENDMACRO(GET_NATIVE_PRECOMPILED_HEADER)

MACRO(ADD_NATIVE_PRECOMPILED_HEADER _targetName _input)

    IF( "${ARGN}" STREQUAL "0")
    SET(_dowarn 0)
    ELSE( "${ARGN}" STREQUAL "0")
    SET(_dowarn 1)
    ENDIF("${ARGN}" STREQUAL "0")

    if(CMAKE_GENERATOR MATCHES Visual*)
    # Auto include the precompile (useful for moc processing, since the use of
    # precompiled is specified at the target level
    # and I don't want to specifiy /F- for each moc/res/ui generated files (using Qt)

    GET_TARGET_PROPERTY(oldProps ${_targetName} COMPILE_FLAGS)
    if (${oldProps} MATCHES NOTFOUND)
        SET(oldProps "")
    endif(${oldProps} MATCHES NOTFOUND)

    SET(newProperties "${oldProps} /Yu\"${_input}.h\" /FI\"${_input}.h\"")
    SET_TARGET_PROPERTIES(${_targetName} PROPERTIES COMPILE_FLAGS "${newProperties}")

    #also inlude ${oldProps} to have the same compile options
    SET_SOURCE_FILES_PROPERTIES(${_input}.cpp PROPERTIES COMPILE_FLAGS "${oldProps} /Yc\"${_input}.h\"")

    else(CMAKE_GENERATOR MATCHES Visual*)

    if (CMAKE_GENERATOR MATCHES Xcode)
        # For Xcode, cmake needs my patch to process
        # GCC_PREFIX_HEADER and GCC_PRECOMPILE_PREFIX_HEADER as target properties

        GET_TARGET_PROPERTY(oldProps ${_targetName} COMPILE_FLAGS)
        if (${oldProps} MATCHES NOTFOUND)
        SET(oldProps "")
        endif(${oldProps} MATCHES NOTFOUND)

        # When buiding out of the tree, precompiled may not be located
        # Use full path instead.
        GET_FILENAME_COMPONENT(fullPath ${_input} ABSOLUTE)

        SET_TARGET_PROPERTIES(${_targetName} PROPERTIES XCODE_ATTRIBUTE_GCC_PREFIX_HEADER "${fullPath}")
        SET_TARGET_PROPERTIES(${_targetName} PROPERTIES XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER "YES")

    else (CMAKE_GENERATOR MATCHES Xcode)

        #Fallback to the "old" precompiled suppport
        #ADD_PRECOMPILED_HEADER(${_targetName} ${_input} ${_dowarn})
    endif(CMAKE_GENERATOR MATCHES Xcode)
    endif(CMAKE_GENERATOR MATCHES Visual*)

ENDMACRO(ADD_NATIVE_PRECOMPILED_HEADER)
