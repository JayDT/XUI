#-------------------------------------------------------------------
# This file is part of the CMake build system for OGRE
#     (Object-oriented Graphics Rendering Engine)
# For the latest info, see http://www.ogre3d.org/
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Find DirectX11 SDK
# Define:
# DirectX11_FOUND
# DirectX11_INCLUDE_DIRS
# DirectX11_LIBRARIES

if(WIN32) # The only platform it makes sense to check for DirectX11 SDK
	include(FindPkgMacros)
	include(IOUtil)
	findpkg_begin(DirectX11)

	if(WINDOWS_PHONE)
        # Windows Phone 8.1 SDK
		find_path(DirectX11_INCLUDE_DIR NAMES d3d11.h HINTS "C:/Program Files (x86)/Windows Phone Kits/8.1/include" "C:/Program Files/Windows Phone Kits/8.1/include")
		set(DirectX11_LIBRARY d3d11.lib dxgi.lib dxguid.lib) # in "C:/Program Files (x86)/Windows Phone Kits/8.1/lib/${MSVC_CXX_ARCHITECTURE_ID}/"

        # Windows Phone 8.0 SDK
		find_path(DirectX11_INCLUDE_DIR NAMES d3d11.h HINTS "C:/Program Files (x86)/Windows Phone Kits/8.0/include" "C:/Program Files/Windows Phone Kits/8.0/include")
		set(DirectX11_LIBRARY d3d11.lib dxgi.lib dxguid.lib) # in "C:/Program Files (x86)/Windows Phone Kits/8.0/lib/${MSVC_CXX_ARCHITECTURE_ID}/"
    
    else(WINDOWS_PHONE)
  
      get_filename_component(windows_kits_dir
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot10]" ABSOLUTE)
      set(programfilesx86 "ProgramFiles(x86)")

        if(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
            set(W10SDK_VER ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION})
        else()
            set(W10SDK_VER 10.0.10069.0)
        endif()
  
        set(DirectX_Include_SUFFIXES
        $ENV{CMAKE_WINDOWS_KITS_10_DIR}
            "Include/${W10SDK_VER}/shared/"
            "Include/${W10SDK_VER}/um/"
            "Include/10.0.10240.0/shared/"
            "Include/10.0.10240.0/um/"
            "Include/10.0.10150.0/shared/"
            "Include/10.0.10150.0/um/"
            "Include/shared/"
            "Include/um/"
            "Include/"
        )
        
        # dlls are in DirectX_ROOT_DIR/Developer Runtime/x64|x86
        # lib files are in DirectX_ROOT_DIR/Lib/x64|x86
        if(CMAKE_CL_64)
            set(DirectX_LIBPATH_SUFFIX
                "Lib/${W10SDK_VER}/um/x64"
                "Lib/10.0.10240.0/um/x64"
                "Lib/10.0.10150.0/um/x64"
                "Lib/winv6.3/um/x64"
                "Lib/win8/um/x64"
                "Lib/x64"
            )
        else(CMAKE_CL_64)
            set(DirectX_LIBPATH_SUFFIX
                "Lib/${W10SDK_VER}/um/x86"
                "Lib/10.0.10240.0/um/x86"
                "Lib/10.0.10150.0/um/x86"
                "Lib/winv6.3/um/x86"
                "Lib/win8/um/x86"
                "Lib/x86"
            )
        endif(CMAKE_CL_64)

        set(DirectX_PREFIX_PATH 
		  "${windows_kits_dir}"
          "c:/Program Files (x86)/Windows Kits/10/"  # Windows 10.0 SDK
          "c:/Program Files (x86)/Windows Kits/8.1/" # Windows 8.1 SDK
          "c:/Program Files (x86)/Windows Kits/8/"   # Windows 8.0 SDK         
          )
		  
		#SUBDIRLIST(RootDir "c:/Program Files (x86)/Windows Kits/10/Lib")
		#LIST(APPEND RootDir ${LocalDir})
		#	
		#SET(BoostDirList "")
		#FOREACH(child ${RootDir})
		#	message(${child})
		#	#IF (${child} MATCHES ".*/boost.*")		
		#	#	LIST(APPEND BoostDirList ${child})
		#	#ENDIF()
		#ENDFOREACH()		  
    
        message(STATUS "[D3D11] Windows SDK System version: ${CMAKE_SYSTEM_VERSION} ...")
        
        find_path(DirectX11_INCLUDE_DIR NAMES d3d11.h HINTS                 ${DirectX_PREFIX_PATH}       PATH_SUFFIXES ${DirectX_Include_SUFFIXES})
        find_path(DirectX11_SHARED_DIR NAMES dxgi.h  HINTS                  ${DirectX_PREFIX_PATH}       PATH_SUFFIXES ${DirectX_Include_SUFFIXES})
        
        find_library(DirectX11_DXERR_LIBRARY NAMES DxErr HINTS              ${DirectX_PREFIX_PATH}       PATH_SUFFIXES ${DirectX_LIBPATH_SUFFIX})
        find_library(DirectX11_DXGUID_LIBRARY NAMES dxguid HINTS            ${DirectX_PREFIX_PATH}       PATH_SUFFIXES ${DirectX_LIBPATH_SUFFIX})
        find_library(DirectX11_DXGI_LIBRARY NAMES dxgi HINTS                ${DirectX_PREFIX_PATH}       PATH_SUFFIXES ${DirectX_LIBPATH_SUFFIX})
        find_library(DirectX11_D3DCOMPILER_LIBRARY NAMES d3dcompiler HINTS  ${DirectX_PREFIX_PATH}       PATH_SUFFIXES ${DirectX_LIBPATH_SUFFIX})
        find_library(DirectX11_D3D11_LIBRARY NAMES d3d11 HINTS              ${DirectX_PREFIX_PATH}       PATH_SUFFIXES ${DirectX_LIBPATH_SUFFIX})
        find_library(DirectX11_D3DX11_LIBRARY NAMES d3dx11 HINTS            ${DirectX_PREFIX_PATH}       PATH_SUFFIXES ${DirectX_LIBPATH_SUFFIX})
    
        if (DirectX11_INCLUDE_DIR AND DirectX11_D3D11_LIBRARY)
            message(STATUS "[D3D11] Found Windows SDK")            
            set(DirectX11_D3D11_LIBRARIES ${DirectX11_D3D11_LIBRARIES}
                ${DirectX11_D3D11_LIBRARY}
                ${DirectX11_DXGI_LIBRARY}
                ${DirectX11_DXGUID_LIBRARY}
                ${DirectX11_D3DCOMPILER_LIBRARY}
            )
        endif ()
        
        if (DirectX11_D3DX11_LIBRARY)
            set(DirectX11_D3D11_LIBRARIES ${DirectX11_D3D11_LIBRARIES} ${DirectX11_D3DX11_LIBRARY})
        endif ()
        
        if (DirectX11_DXERR_LIBRARY)
            set(DirectX11_D3D11_LIBRARIES ${DirectX11_D3D11_LIBRARIES} ${DirectX11_DXERR_LIBRARY})
        endif ()
            
        # Legacy Direct X SDK
        if (NOT DirectX11_INCLUDE_DIR) 

            message(STATUS "[D3D11] Legacy Direct X SDK System versio: ${CMAKE_SYSTEM_VERSION} ...")
        
            # Get path, convert backslashes as ${ENV_DXSDK_DIR}
            getenv_path(DXSDK_DIR)
            getenv_path(DIRECTX_HOME)
            getenv_path(DIRECTX_ROOT)
            getenv_path(DIRECTX_BASE)
        
            # construct search paths
            set(DirectX11_PREFIX_PATH 
            "${DXSDK_DIR}" "${ENV_DXSDK_DIR}"
            "${DIRECTX_HOME}" "${ENV_DIRECTX_HOME}"
            "${DIRECTX_ROOT}" "${ENV_DIRECTX_ROOT}"
            "${DIRECTX_BASE}" "${ENV_DIRECTX_BASE}"
            "C:/apps_x86/Microsoft DirectX SDK*"
            "C:/Program Files (x86)/Microsoft DirectX SDK*"
            "C:/apps/Microsoft DirectX SDK*"
            "C:/Program Files/Microsoft DirectX SDK*"
            "C:/Program Files (x86)/Windows Kits/8.1/Include/um"
            "$ENV{ProgramFiles}/Microsoft DirectX SDK*"
            )
        
            create_search_paths(DirectX11)
            # redo search if prefix path changed
            clear_if_changed(DirectX11_PREFIX_PATH
                DirectX11_LIBRARY
                DirectX11_INCLUDE_DIR
            )
        
            # dlls are in DirectX11_ROOT_DIR/Developer Runtime/x64|x86
            # lib files are in DirectX11_ROOT_DIR/Lib/x64|x86
            if(CMAKE_CL_64)
                set(DirectX11_LIBPATH_SUFFIX "x64")
            else(CMAKE_CL_64)
                set(DirectX11_LIBPATH_SUFFIX "x86")
            endif(CMAKE_CL_64)
        
            # look for D3D11 components
            find_path(DirectX11_INCLUDE_DIR NAMES d3d11.h HINTS                 ${DirectX11_INC_SEARCH_PATH})
            find_library(DirectX11_DXERR_LIBRARY NAMES DxErr HINTS              ${DirectX11_LIB_SEARCH_PATH} PATH_SUFFIXES ${DirectX11_LIBPATH_SUFFIX})
            find_library(DirectX11_DXGUID_LIBRARY NAMES dxguid HINTS            ${DirectX11_LIB_SEARCH_PATH} PATH_SUFFIXES ${DirectX11_LIBPATH_SUFFIX})
            find_library(DirectX11_DXGI_LIBRARY NAMES dxgi HINTS                ${DirectX11_LIB_SEARCH_PATH} PATH_SUFFIXES ${DirectX11_LIBPATH_SUFFIX})
            find_library(DirectX11_D3DCOMPILER_LIBRARY NAMES d3dcompiler HINTS  ${DirectX11_LIB_SEARCH_PATH} PATH_SUFFIXES ${DirectX11_LIBPATH_SUFFIX})
            find_library(DirectX11_D3D11_LIBRARY NAMES d3d11 HINTS              ${DirectX11_LIB_SEARCH_PATH} PATH_SUFFIXES ${DirectX11_LIBPATH_SUFFIX})
            find_library(DirectX11_D3DX11_LIBRARY NAMES d3dx11 HINTS            ${DirectX11_LIB_SEARCH_PATH} PATH_SUFFIXES ${DirectX11_LIBPATH_SUFFIX})	
        
            if (DirectX11_INCLUDE_DIR AND DirectX11_D3D11_LIBRARY)
                message(STATUS "[D3D11] Found Legacy DirectX SDK")            
                set(DirectX11_D3D11_LIBRARIES ${DirectX11_D3D11_LIBRARIES}
                    ${DirectX11_D3D11_LIBRARY}
                    ${DirectX11_DXGI_LIBRARY}
                    ${DirectX11_DXGUID_LIBRARY}
                    ${DirectX11_D3DCOMPILER_LIBRARY}
                )
            endif ()
            if (DirectX11_D3DX11_LIBRARY)
                set(DirectX11_D3D11_LIBRARIES ${DirectX11_D3D11_LIBRARIES} ${DirectX11_D3DX11_LIBRARY})
            endif ()
            if (DirectX11_DXERR_LIBRARY)
                set(DirectX11_D3D11_LIBRARIES ${DirectX11_D3D11_LIBRARIES} ${DirectX11_DXERR_LIBRARY})
            endif ()
        endif(NOT DirectX11_INCLUDE_DIR) # Legacy Direct X SDK

        set(DirectX11_INCLUDE
            ${DirectX11_INCLUDE_DIR}        
            ${DirectX11_SHARED_DIR} 
        )
        
        set(DirectX11_LIBRARY 
            ${DirectX11_D3D11_LIBRARIES} 
        )
        
        mark_as_advanced(DirectX11_D3D11_LIBRARY 
                        DirectX11_D3DX11_LIBRARY
                        DirectX11_DXERR_LIBRARY 
                        DirectX11_DXGUID_LIBRARY
                        DirectX11_DXGI_LIBRARY 
                        DirectX11_D3DCOMPILER_LIBRARY)	

	endif (WINDOWS_PHONE)

	findpkg_finish(DirectX11)
	
endif(WIN32)