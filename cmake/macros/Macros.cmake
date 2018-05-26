MACRO(SETUP_EDIT_AND_CONTINUE PROJECT_NAME HAVE_LINKER)
  message(${LibExSTATUS} "Configured Edit and Continue on ${PROJECT_NAME} ${HAVE_LINKER}")
  if(MSVC)
    #add_definitions(/ZI)
    if(${HAVE_LINKER} EQUAL 1)
        message(${LibExSTATUS} "Configured Linker Edit and Continue on ${PROJECT_NAME}")
        #target_link_libraries(${PROJECT_NAME} "-SAFESEH:NO") #"-SAFESEH:NO"  "$<$<CXX_COMPILER_ID:MSVC>:-SAFESEH:NO>"
        #target_link_libraries(${PROJECT_NAME} "-OPT:NOLBR") #"-SAFESEH:NO"  "$<$<CXX_COMPILER_ID:MSVC>:-SAFESEH:NO>"
    endif()
  endif()
ENDMACRO(SETUP_EDIT_AND_CONTINUE)

MACRO(ADD_VS_NATIVE_PRECOMPILED_HEADER _targetName _inputh _inputcpp)
    IF(CMAKE_GENERATOR MATCHES Visual*)
        # Auto include the precompile (useful for moc processing, since the use of
        # precompiled is specified at the target level
        # and I don't want to specifiy /F- for each moc/res/ui generated files (using Qt)

        GET_TARGET_PROPERTY(oldProps ${_targetName} COMPILE_FLAGS)
        IF(${oldProps} MATCHES NOTFOUND)
            SET(oldProps "")
        ENDIF(${oldProps} MATCHES NOTFOUND)

        #SET(newProperties "${oldProps} /Yu\"${_inputh}\" /FI\"${_inputh}\"")
        SET(newProperties "${oldProps} /Yu\"${_inputh}\"")
        SET_TARGET_PROPERTIES(${_targetName} PROPERTIES COMPILE_FLAGS "${newProperties}")

        #also inlude ${oldProps} to have the same compile options
        SET_SOURCE_FILES_PROPERTIES(${_inputcpp} PROPERTIES COMPILE_FLAGS "${oldProps} /Yc\"${_inputh}\"")
    ELSE(CMAKE_GENERATOR MATCHES Visual*)
        IF(CMAKE_GENERATOR MATCHES Xcode)
            # For Xcode, cmake needs my patch to process
            # GCC_PREFIX_HEADER and GCC_PRECOMPILE_PREFIX_HEADER as target properties

            GET_TARGET_PROPERTY(oldProps ${_targetName} COMPILE_FLAGS)
            IF(${oldProps} MATCHES NOTFOUND)
                SET(oldProps "")
            ENDIF(${oldProps} MATCHES NOTFOUND)

            # When buiding out of the tree, precompiled may not be located
            # Use full path instead.
            GET_FILENAME_COMPONENT(fullPath ${_inputh} ABSOLUTE)

            SET_TARGET_PROPERTIES(${_targetName} PROPERTIES XCODE_ATTRIBUTE_GCC_PREFIX_HEADER "${fullPath}")
            SET_TARGET_PROPERTIES(${_targetName} PROPERTIES XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER "YES")
        ELSE(CMAKE_GENERATOR MATCHES Xcode)
            #Fallback to the "old" precompiled suppport
            ADD_PRECOMPILED_HEADER(${_targetName} ${_inputh} ${_inputcpp})
        ENDIF(CMAKE_GENERATOR MATCHES Xcode)
    ENDIF(CMAKE_GENERATOR MATCHES Visual*)
ENDMACRO(ADD_VS_NATIVE_PRECOMPILED_HEADER)

MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader EXECUTABLE_NAME)
  message(${ASTATUS} "World Precomplined header")
  if(CMAKE_COMPILER_IS_GNUCXX)
    add_precompiled_header(EXECUTABLE_NAME  PrecompiledHeader)
  elseif(MSVC)
    add_native_precompiled_header(EXECUTABLE_NAME PrecompiledHeader)
  endif()
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        LIST(APPEND dirlist ${curdir}/${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

FUNCTION(CreateLibrary LibName LibSource Linkage Dependencis LibLinkFlags)

    add_library(${LibName} ${Linkage} ${LibSource} )
    
    IF(EAC)
        IF (${Linkage} STREQUAL "SHARED")
            SETUP_EDIT_AND_CONTINUE("${LibName}" 1)
        ELSE()
            SETUP_EDIT_AND_CONTINUE("${LibName}" 0)
        ENDIF()
    ENDIF(EAC)

    IF (${Linkage} STREQUAL "SHARED")
        target_link_libraries(${LibName}
          ${Dependencis}
        )

        #set_target_properties(${LibName} PROPERTIES LINK_FLAGS
        #  "${LibLinkFlags}"
        #)
        
        # LIBRARY = dyld / so, RUNTIME = dll
        if( UNIX )
          install(TARGETS ${LibName} LIBRARY DESTINATION "${LIBS_DIR}")
        elseif( WIN32 )
          install(TARGETS ${LibName} RUNTIME DESTINATION "${LIBS_DIR}")
        endif(UNIX)
        
        message(${DllSTATUS} ${LibName})		
    ELSE()
        message(${LibSTATUS} ${LibName})
    ENDIF(${Linkage} STREQUAL "SHARED")
    
ENDFUNCTION()

FUNCTION(CreateHotSwap LibName LibSource Linkage Dependencis LibLinkFlags)

    add_library(${LibName} ${Linkage} ${LibSource} )
    
    # Edit and Continue block pdb
    #IF(EAC)
    #    IF (${Linkage} STREQUAL "SHARED")
    #        SETUP_EDIT_AND_CONTINUE("${LibName}" 1)
    #    ELSE()
    #        SETUP_EDIT_AND_CONTINUE("${LibName}" 0)
    #    ENDIF()
    #ENDIF(EAC)
    
    # Block pdb
    #if(WFL AND WIN32)
    #  set(LibLinkFlags "${LibLinkFlags} /DEBUG:FASTLINK")
    #endif(WFL AND WIN32)

    IF (${Linkage} STREQUAL "SHARED")
        target_link_libraries(${LibName} ${Dependencis} )

        #set_target_properties(${LibName} PROPERTIES LINK_FLAGS
        #  "${LibLinkFlags}"
        #)

        if( WIN32 )
          IF(EAC)
            string(REPLACE "/ZI" "" DowngradedCompileFlags ${CMAKE_CXX_FLAGS_DEBUG})
            set_target_properties(${LibName} PROPERTIES COMPILE_FLAGS "${DowngradedCompileFlags}" )
          ENDIF(EAC)
          target_link_libraries(${LibName} "-DEBUG") # optimize for dynamic pdb
          #target_link_libraries(${LibName} "-INCREMENTAL:NO") # optimize for dynamic pdb
          target_link_libraries(${LibName} "-PDBALTPATH:%_PDB%")
          set_target_properties(${LibName} PROPERTIES PDB_NAME "$(TargetName)-$([System.DateTime]::Now.ToString(\"HH_mm_ss_fff\"))")
          add_custom_command(TARGET ${LibName} PRE_LINK COMMAND del $(OutDir)$(MSBuildProjectName)-*.pdb COMMENT "Delete pdb files")
        endif( WIN32 )
        
        # LIBRARY = dyld / so, RUNTIME = dll
        if( UNIX )
          install(TARGETS ${LibName} LIBRARY DESTINATION "${BIN_DIR}")
        elseif( WIN32 )
          install(TARGETS ${LibName} RUNTIME DESTINATION "${BIN_DIR}")
        endif(UNIX)

        message(${DllSTATUS} ${LibName} "  (HotSwap)")
    ELSE()
        message(${LibSTATUS} ${LibName})
    ENDIF(${Linkage} STREQUAL "SHARED")

ENDFUNCTION()

FUNCTION(CreateExecutable LibName LibSource Dependencis LibLinkFlags)

    add_executable(${LibName} ${LibSource} )
    
    IF(EAC)
        SETUP_EDIT_AND_CONTINUE("${LibName}" 1)
    ENDIF(EAC)

    target_link_libraries(${LibName}
        ${Dependencis}
    )

    #set_target_properties(${LibName} PROPERTIES LINK_FLAGS
    #	"${LibLinkFlags}"
    #)

    if( UNIX )
        install(TARGETS ${LibName} RUNTIME DESTINATION "${BIN_DIR}")
    elseif( WIN32 )
        install(TARGETS ${LibName} RUNTIME DESTINATION "${BIN_DIR}")
    endif(UNIX)
    
    message(${BinSTATUS} ${LibName})
ENDFUNCTION()

FUNCTION(CreateCxxPch LibName Header Source)
    # Generate precompiled header
    if( PCH )
        message(${LibExSTATUS} "${LibName} Precomplined header")

        set(PRIVATE_PCH_HEADER ${Header})
        set(PRIVATE_PCH_SOURCE ${Source})

        ADD_CXX_PCH(${LibName} ${PRIVATE_PCH_HEADER} ${PRIVATE_PCH_SOURCE})
        # Old style
        #if(CMAKE_COMPILER_IS_GNUCXX)
        #	add_precompiled_header(${LibName} ${Header})
        #elseif(MSVC)
        #	add_cxx_pch(${LibName} ${Header} ${Source})
        #endif()
    endif()
ENDFUNCTION()

# Collects all subdirectoroies into the given variable,
# which is useful to include all subdirectories.
# Ignores full qualified directories listed in the variadic arguments.
#
# Use it like:
# CollectIncludeDirectories(
#   ${CMAKE_CURRENT_SOURCE_DIR}
#   COMMON_PUBLIC_INCLUDES
#   # Exclude
#   ${CMAKE_CURRENT_SOURCE_DIR}/PrecompiledHeaders
#   ${CMAKE_CURRENT_SOURCE_DIR}/Platform)
#
FUNCTION(CollectIncludeDirectories current_dir variable)
  list(FIND ARGN "${current_dir}" IS_EXCLUDED)
  #MESSAGE("Excluded ${current_dir} on ${ARGN}")
  if(IS_EXCLUDED EQUAL -1)
    list(APPEND ${variable} " ${current_dir}")
    file(GLOB SUB_DIRECTORIES ${current_dir}/*)
    foreach(SUB_DIRECTORY ${SUB_DIRECTORIES})
      if (IS_DIRECTORY ${SUB_DIRECTORY})
        CollectIncludeDirectories("${SUB_DIRECTORY}" "${variable}" "${ARGN}")
      endif()
    endforeach()
    set(${variable} ${${variable}} PARENT_SCOPE)
  endif()
ENDFUNCTION()

FUNCTION(AppendDefaultIncludeDir)
    CollectIncludeDirectories("${CMAKE_CURRENT_SOURCE_DIR}" PUBLIC_INCLUDES "${ARGN}")

    include_directories(
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${PUBLIC_INCLUDES}
      ${CMAKE_SOURCE_DIR}
      ${CMAKE_SOURCE_DIR}/src
      ${CMAKE_SOURCE_DIR}/src/CFramework
      ${CMAKE_SOURCE_DIR}/src/XUI
      ${CMAKE_SOURCE_DIR}/src/IrrLicht
      ${CMAKE_SOURCE_DIR}/src/IrrLicht/include
	  ${CMAKE_SOURCE_DIR}/src/Reflection
	  ${CMAKE_SOURCE_DIR}/src/Reflection/Runtime/Common/Lib
      ${CMAKE_SOURCE_DIR}/dep
      ${CMAKE_SOURCE_DIR}/dep/cppformat
      ${CMAKE_SOURCE_DIR}/dep/Reactive
      ${CMAKE_SOURCE_DIR}/dep/zlib
      ${CMAKE_SOURCE_DIR}/dep/process
      ${CMAKE_SOURCE_DIR}/dep/sparse
      ${CMAKE_BINARY_DIR}
      ${ZLIB_INCLUDE_DIR}
      ${Boost_INCLUDE_DIRS}
    )
    
    IF(UNIX)
        include_directories( 
            ${LIBDL_INCLUDE_DIR}
            ${BFD_INCLUDE_DIR}
          )
    ENDIF(UNIX)

ENDFUNCTION()