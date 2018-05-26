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
${DirectX9_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}
)

file(GLOB_RECURSE DirectX9_Source RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.c *.h *.hpp *.hlsl)

setup_hlsl(${DirectX9_Source})

CreateLibrary("DirectX9" "${DirectX9_Source}" "${LIBTYPE}" "" "")

add_dependencies(DirectX9 zlib)

if(_IRR_DYNAMIC_)
    target_link_libraries (DirectX9 ${DirectX_LIBRARY} ${DirectX_D3DX9_LIBRARY} ${DirectX_DXGUID_LIBRARY} Winmm zlib)
    install(TARGETS DirectX9 DESTINATION ${CMAKE_INSTALL_PREFIX})
else()
    add_definitions(-D_LIB_SOURCE_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")
    add_definitions(-D_IRR_COMPILE_WITH_DIRECT3D_9_)
    add_definitions(-DIRRLICHT_EXPORTS)
endif(_IRR_DYNAMIC_)

set_target_properties(DirectX9
    PROPERTIES
      FOLDER
        "RenderEngine/IrrLicht")