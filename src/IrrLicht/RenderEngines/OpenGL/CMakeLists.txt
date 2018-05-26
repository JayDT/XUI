set(_IRR_DYNAMIC_ false)

if(_IRR_DYNAMIC_)
  set(LIBTYPE SHARED)
else(_IRR_DYNAMIC_)
 set(LIBTYPE STATIC)
endif(_IRR_DYNAMIC_)

include_directories(
${CMAKE_CURRENT_SOURCE_DIR}
${OPENGL_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/dep/WInclude/zlib
${CMAKE_SOURCE_DIR}/dep/WInclude
${CMAKE_SOURCE_DIR}/dep/include
${CMAKE_SOURCE_DIR}/contrib/IrrLicht
${CMAKE_SOURCE_DIR}/contrib/IrrLicht/include
${CMAKE_SOURCE_DIR}
)

file(GLOB_RECURSE OpenGL_Source RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.c *.h *.hpp *.hlsl)

#setup_glsl(${OpenGL_Source})

CreateLibrary("OpenGL" "${OpenGL_Source}" "${LIBTYPE}" "" "")

add_dependencies(OpenGL zlib)

if(_IRR_DYNAMIC_)
    target_link_libraries (OpenGL ${OPENGL0_LIBRARY} ${OPENGL1_LIBRARY} Winmm zlib)
    install(TARGETS OpenGL DESTINATION ${CMAKE_INSTALL_PREFIX})
else()
    add_definitions(-D_LIB_SOURCE_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")
    add_definitions(-D_IRR_COMPILE_WITH_OPENGL_)
    add_definitions(-DIRRLICHT_EXPORTS)
endif(_IRR_DYNAMIC_)

set_target_properties(OpenGL
    PROPERTIES
      FOLDER
        "RenderEngine/IrrLicht")