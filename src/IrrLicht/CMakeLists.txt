# Init variables
set(RENDER_ENGINE_DEFINES "")

set(_IRR_DYNAMIC_ true)

if(_IRR_DYNAMIC_)
  set(LIBTYPE SHARED)
else(_IRR_DYNAMIC_)
  set(LIBTYPE STATIC)
  set(RENDER_ENGINE_DEFINES "${RENDER_ENGINE_DEFINES} -D_IRR_STATIC_LIB_=1" )
endif(_IRR_DYNAMIC_)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../dep
    ${CMAKE_CURRENT_SOURCE_DIR}/../../dep/jpeglib
    ${CMAKE_CURRENT_SOURCE_DIR}/../../dep/libpng
    ${CMAKE_CURRENT_SOURCE_DIR}/../../dep/freetype
    ${CMAKE_CURRENT_SOURCE_DIR}/../../dep/freetype/include
    ${ZLIB_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}
)

AppendDefaultIncludeDir(${CMAKE_CURRENT_SOURCE_DIR}/standard)

file(GLOB IrrLicht_source RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.c *.h *.hpp *.hlsl)

list(APPEND IrrLicht_source

	#${IrrLicht_header}

    aesGladman/aescrypt.cpp
    aesGladman/aeskey.cpp
    aesGladman/aestab.cpp
    aesGladman/fileenc.cpp
    aesGladman/hmac.cpp
    aesGladman/prng.cpp
    aesGladman/pwd2key.cpp
    aesGladman/sha1.cpp
    aesGladman/sha2.cpp
    
    bzip2/blocksort.c
    bzip2/bzlib.c
    bzip2/crctable.c
    bzip2/decompress.c
    bzip2/bzcompress.c
    bzip2/huffman.c
    bzip2/randtable.c
    
    lzma/LzmaDec.c

    util/extra_string.cpp
)

source_group("video\\OpenGL"
  REGULAR_EXPRESSION OpenGL
)

source_group("video\\Direct3D9"
  REGULAR_EXPRESSION D3D9
)

source_group("video\\Direct3D11"
  REGULAR_EXPRESSION D3D11
)

source_group("video\\Direct3D11\\Shaders"
  REGULAR_EXPRESSION hlsl
)

source_group("video\\Software"
  REGULAR_EXPRESSION .*\(CSoftware|CTR\).*
)

source_group("video\\Null"
  REGULAR_EXPRESSION .*\(CNull|CColor\).*
)

source_group("video\\Burning Video"
  REGULAR_EXPRESSION .*\(Burning\).*
)

source_group("gui"
  REGULAR_EXPRESSION CGUI
)

source_group("scene"
  REGULAR_EXPRESSION SceneNode|scenenode
)

source_group("scene\\emitter"
  REGULAR_EXPRESSION Emitter|emitter
)

source_group("mesh"
  REGULAR_EXPRESSION Mesh|mesh
)

source_group("mesh\\File"
  REGULAR_EXPRESSION MeshFile|meshfile
)

source_group("System\\Device"
  REGULAR_EXPRESSION Device|device
)

source_group("Image"
  REGULAR_EXPRESSION CImage
)

foreach(SRC ${IrrLicht_source})
  get_filename_component(PTH ${SRC} PATH)
  if(PTH)
    if(NOT XCODE) # FIXME: Xcode Generator has bug with nested dirs
      string(REPLACE "/" "\\\\" PTH ${PTH})
    endif()
    source_group(${PTH} FILES ${SRC})
  endif()
endforeach(SRC)

CreateLibrary("irrlicht" "${IrrLicht_source}" "${LIBTYPE}" "" "")

add_dependencies(irrlicht zlib)
add_subdirectory(RenderEngines)

#! Only define _IRR_COMPILE_WITH_DIRECT3D_8_ if you have an appropriate DXSDK, e.g. Summer 2004
if (IRR_BUILD_RENDERSYSTEM_D3DX8 EQUAL 1)
    set(RENDER_ENGINE_DEFINES "${RENDER_ENGINE_DEFINES} -D_IRR_COMPILE_WITH_DIRECT3D_8_" )
endif(IRR_BUILD_RENDERSYSTEM_D3DX8 EQUAL 1)

if (IRR_BUILD_RENDERSYSTEM_D3DX9 EQUAL 1)
    set(RENDER_ENGINE_DEFINES "${RENDER_ENGINE_DEFINES} -D_IRR_COMPILE_WITH_DIRECT3D_9_" )
endif(IRR_BUILD_RENDERSYSTEM_D3DX9 EQUAL 1)

if (IRR_BUILD_RENDERSYSTEM_D3D11 EQUAL 1)
    set(RENDER_ENGINE_DEFINES "${RENDER_ENGINE_DEFINES} -D_IRR_COMPILE_WITH_DIRECT3D_11_" )
endif(IRR_BUILD_RENDERSYSTEM_D3D11 EQUAL 1)

if (IRR_BUILD_RENDERSYSTEM_GL EQUAL 1)
    set(RENDER_ENGINE_DEFINES "${RENDER_ENGINE_DEFINES} -D_IRR_COMPILE_WITH_OPENGL_" )
endif(IRR_BUILD_RENDERSYSTEM_GL EQUAL 1)

SET_TARGET_PROPERTIES (irrlicht PROPERTIES COMPILE_FLAGS "${RENDER_ENGINE_DEFINES}" )
add_definitions(-D_LIB_SOURCE_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")

if(_IRR_DYNAMIC_)
  SET_TARGET_PROPERTIES (irrlicht PROPERTIES DEFINE_SYMBOL  "IRRLICHT_EXPORTS" )
  
  if (IRR_BUILD_RENDERSYSTEM_GL EQUAL 1)
      message(${LibExSTATUS} "Irr: Use OpenGL")
      target_link_libraries (irrlicht 
      OpenGL)
      target_link_libraries (irrlicht
        ${OPENGL0_LIBRARY}
        ${OPENGL1_LIBRARY})          
  endif(IRR_BUILD_RENDERSYSTEM_GL EQUAL 1)
  
  #! Only define _IRR_COMPILE_WITH_DIRECT3D_8_ if you have an appropriate DXSDK, e.g. Summer 2004
  if (IRR_BUILD_RENDERSYSTEM_D3DX8 EQUAL 1)
      message(${LibExSTATUS} "Irr: Use DX8")
      target_link_libraries (irrlicht 
      DirectX8)
      target_link_libraries (irrlicht
        ${DirectX8_LIBRARIES})                 
  endif(IRR_BUILD_RENDERSYSTEM_D3DX8 EQUAL 1)

  if (IRR_BUILD_RENDERSYSTEM_D3DX9 EQUAL 1)
      message(${LibExSTATUS} "Irr: Use DX9")
      target_link_libraries (irrlicht 
      DirectX9)
      target_link_libraries (irrlicht
        ${DirectX9_LIBRARIES})          
  endif(IRR_BUILD_RENDERSYSTEM_D3DX9 EQUAL 1)
  
  if (IRR_BUILD_RENDERSYSTEM_D3D11 EQUAL 1)
      message(${LibExSTATUS} "Irr: Use D3D11")
      target_link_libraries (irrlicht 
      Direct3D11)
      target_link_libraries (irrlicht
        ${DirectX11_LIBRARY} )          
  endif(IRR_BUILD_RENDERSYSTEM_D3D11 EQUAL 1)
   
  target_link_libraries (irrlicht
	g3dlite
    CFramework
    freetype
	MetaRuntime
	libjpeg
	libpng
    ${ZLIB_LIBRARIES}
    )

  install(TARGETS irrlicht DESTINATION ${CMAKE_INSTALL_PREFIX}) # irrlicht needs to go to the install dir on windows as it's a dll
endif(_IRR_DYNAMIC_)

set_target_properties(irrlicht
    PROPERTIES
      FOLDER
        "RenderEngine")