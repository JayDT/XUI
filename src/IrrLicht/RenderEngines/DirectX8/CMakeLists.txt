include(HlslUtils)

set(_IRR_DYNAMIC_ false)

if(_IRR_DYNAMIC_)
  set(LIBTYPE SHARED)
else(_IRR_DYNAMIC_)
 set(LIBTYPE STATIC)
endif(_IRR_DYNAMIC_)

include_directories(
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_SOURCE_DIR}/dep/WInclude/zlib
${CMAKE_SOURCE_DIR}/dep/WInclude
${CMAKE_SOURCE_DIR}/dep/include
${CMAKE_SOURCE_DIR}/contrib/IrrLicht
${CMAKE_SOURCE_DIR}/contrib/IrrLicht/include
${CMAKE_SOURCE_DIR}
)

file(GLOB_RECURSE DirectX8_Source RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.c *.h *.hpp *.hlsl)

#setup_hlsl(${DirectX8_Source})

CreateLibrary("DirectX8" "${DirectX8_Source}" "${LIBTYPE}" "" "")

add_dependencies(DirectX8 zlib)

if(_IRR_DYNAMIC_)
    target_link_libraries (DirectX8 ${DirectX_LIBRARY} ${DirectX_D3DX8_LIBRARY} ${DirectX_DXGUID_LIBRARY} Winmm zlib)
    install(TARGETS DirectX8 DESTINATION ${CMAKE_INSTALL_PREFIX})
else()
    add_definitions(-D_LIB_SOURCE_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")
    add_definitions(-D_IRR_COMPILE_WITH_DIRECT3D_8_)
    add_definitions(-DIRRLICHT_EXPORTS)
endif(_IRR_DYNAMIC_)

set_target_properties(DirectX8
    PROPERTIES
      FOLDER
        "RenderEngine/IrrLicht")