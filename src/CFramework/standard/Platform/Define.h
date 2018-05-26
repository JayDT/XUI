/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef MODUL_DEFINE_H
#define MODUL_DEFINE_H

#include <cstdint>
#include <sys/types.h>
#include "standard/Platform/CompilerDefs.h"

#define MODUL_LITTLEENDIAN 0
#define MODUL_BIGENDIAN    1

#if defined ( WIN32 )
#define __func__ __FUNCTION__
#endif

#define STRINGIFY(a) #a

#if !defined(MODUL_ENDIAN)
#  if defined (ACE_BIG_ENDIAN)
#    define MODUL_ENDIAN MODUL_BIGENDIAN
#  else //ACE_BYTE_ORDER != ACE_BIG_ENDIAN
#    define MODUL_ENDIAN MODUL_LITTLEENDIAN
#  endif //ACE_BYTE_ORDER
#endif //MODUL_ENDIAN

#ifndef HANDLE
typedef void *HANDLE;
#endif

typedef HANDLE MODUL_LIBRARY_HANDLE;

#define MODUL_SCRIPT_NAME "mscript"
#define MODUL_SCRIPT_SUFFIX ACE_DLL_SUFFIX
#define MODUL_SCRIPT_PREFIX ACE_DLL_PREFIX
#define MODUL_LOAD_LIBRARY(libname)    ACE_OS::dlopen(libname)
#define MODUL_CLOSE_LIBRARY(hlib)      ACE_OS::dlclose(hlib)
#define MODUL_GET_PROC_ADDR(hlib,name) ACE_OS::dlsym(hlib,name)

#define MODUL_PATH_MAX 1024

#if PLATFORM == PLATFORM_WINDOWS
#define REFLECTION_EXPORT extern "C" __declspec(dllexport)
#define REFLECTION_IMPORT __cdecl
#else //*nix part
#define REFLECTION_EXPORT extern "C"
#ifdef __x86_64__
#define REFLECTION_IMPORT 
#else
#define REFLECTION_IMPORT __attribute__ ((cdecl))
#endif
#endif

#if defined(_WIN32)
# if defined(_WIN64)
#	define FORCE_UNDEFINED_SYMBOL(x) __pragma(comment (linker, "/export:" #x))
# else
#	define FORCE_UNDEFINED_SYMBOL(x) __pragma(comment (linker, "/export:_" #x))
# endif
#else
#	define FORCE_UNDEFINED_SYMBOL(x) extern "C" void x(void); void (*__ ## x ## _fp)(void)=&x;
#endif

#pragma region Dynamic Export Defines


#if COMPILER == COMPILER_MICROSOFT
#  define TC_API_EXPORT __declspec(dllexport)
#  define TC_API_IMPORT __declspec(dllimport)
#elif COMPILER == COMPILER_GNU
#  define TC_API_EXPORT __attribute__((visibility("default")))
#  define TC_API_IMPORT                                       
#else
#  error compiler not supported!
#endif

#if PLATFORM == PLATFORM_WINDOWS
#    define TC_EXPORT __declspec(dllexport)
#    define TC_IMPORT __cdecl
#else //PLATFORM != PLATFORM_WINDOWS __attribute__ ((visibility("default")))
#  define TC_EXPORT __attribute__((visibility("default"))) 
#  if defined(__APPLE_CC__) && defined(BIG_ENDIAN)
#    define TC_IMPORT __attribute__ ((longcall))
#  elif defined(__x86_64__)
#    define TC_IMPORT
#  else
#    define TC_IMPORT __attribute__ ((cdecl))
#  endif //__APPLE_CC__ && BIG_ENDIAN
#endif //PLATFORM

#if PLATFORM == PLATFORM_WINDOWS
#  ifdef MODUL_WIN32_DLL_IMPORT
#    define MODUL_DLL_DECL TC_API_IMPORT //__declspec(dllimport)
#  else //!MODUL_WIN32_DLL_IMPORT
#    ifdef MODUL_WIND_DLL_EXPORT
#      define MODUL_DLL_DECL TC_API_EXPORT //__declspec(dllexport)
#    else //!MODUL_WIND_DLL_EXPORT
#      define MODUL_DLL_DECL
#    endif //MODUL_WIND_DLL_EXPORT
#  endif //MODUL_WIN32_DLL_IMPORT
#else //PLATFORM != PLATFORM_WINDOWS
#  define MODUL_DLL_DECL
#endif //PLATFORM

#if PLATFORM == PLATFORM_WINDOWS
#  define MODUL_DLL_SPEC TC_EXPORT
#  ifndef DECLSPEC_NORETURN
#    define DECLSPEC_NORETURN __declspec(noreturn)
#  endif //DECLSPEC_NORETURN
#else //PLATFORM != PLATFORM_WINDOWS
#  define MODUL_DLL_SPEC
#  define DECLSPEC_NORETURN
#endif //PLATFORM

#if COMPILER == COMPILER_GNU
#  define FORMAT_STRING(p) p
#  define ATTR_NORETURN __attribute__((noreturn))
#  define ATTR_PRINTF(F,V) __attribute__ ((format (printf, F, V)))
#else //COMPILER != COMPILER_GNU
#  include <sal.h>
#  define FORMAT_STRING(p) _In_z_ _Printf_format_string_ p
#  define ATTR_NORETURN
#  define ATTR_PRINTF(F,V)
#endif //COMPILER == COMPILER_GNU

#if defined(IGNORE_DEPRECATED_WARNING)
#define _DEPRECATED_
#elif _MSC_VER >= 1310 //vs 2003 or higher
#define DEPRECATED_ __declspec(deprecated)
#elif (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)) // all versions above 3.0 should support this feature
#define DEPRECATED_  __attribute__ ((deprecated))
#else
#define DEPRECATED_
#endif

//! Defines an override macro, to protect virtual functions from typos and other mismatches
/** Usage in a derived class:
virtual void somefunc() _OVERRIDE_;
*/
#if ( ((__GNUC__ > 4 ) || ((__GNUC__ == 4 ) && (__GNUC_MINOR__ >= 7))) && (defined(__GXX_EXPERIMENTAL_CXX0X) || __cplusplus >= 201103L) )
#define OVERRIDE_ override
#elif (_MSC_VER >= 1600 ) /* supported since MSVC 2010 */
#define OVERRIDE_ override
#elif (__clang_major__ >= 3)
#define OVERRIDE_ override
#else
#define OVERRIDE_
#endif

// GCC have alternative #pragma pack(N) syntax and old gcc version not support pack(push,N), also any gcc version not support it at some platform
#if PLATFORM == PLATFORM_WINDOWS
#  define PACK_BEGIN(x) __pragma(pack(push, x))
#  define PACK_END __pragma(pack(pop))
#else //PLATFORM != PLATFORM_WINDOWS
#  define PACK_BEGIN(x) _Pragma("pack(" ## STRINGIFY(x) ## ")")
#  define PACK_END _Pragma("pack()")
#endif //PLATFORM

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;
typedef uint64_t uint64;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef unsigned char byte;
typedef char sbyte;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef void* handle;

#if _MSC_VER
typedef unsigned __int64 uint64_t;
#endif

#if COMPILER != COMPILER_MICROSOFT
typedef uint16      WORD;
typedef uint32      DWORD;
typedef uint64      QWORD;

#  ifndef __forceinline
#      define __forceinline __attribute__((__always_inline__)) inline
#  endif
#else

#   ifndef __REFLECTION_PARSER__
#   include <windows.h>
#   endif

#undef min
#undef max
#endif //COMPILER

#       ifdef GetObject
#       undef GetObject
#       endif
#       ifdef FindResource
#       undef FindResource
#       endif

/// -- Modul Spec Defines

#ifdef TC_DYNAMIC_METARUNTIME
#ifdef TC_API_EXPORT_MetaRuntime
#  define TC_METARUNTIME_API TC_API_EXPORT
#else
#  define TC_METARUNTIME_API TC_API_IMPORT
#endif
#else
#  define TC_METARUNTIME_API MODUL_DLL_DECL
#endif

#ifdef TC_DYNAMIC_CFRAMEWORK
#ifdef TC_API_EXPORT_CFRAMEWORK
#  define TC_CFRAMEWORK_API TC_API_EXPORT
#else
#  define TC_CFRAMEWORK_API TC_API_IMPORT
#endif
#else
#  define TC_CFRAMEWORK_API MODUL_DLL_DECL
#endif

#ifdef TC_DYNAMIC_XUI
#ifdef TC_API_EXPORT_XUI
#  define TC_XUI_API TC_API_EXPORT
#else
#  define TC_XUI_API TC_API_IMPORT
#endif
#else
#  define TC_XUI_API MODUL_DLL_DECL
#endif

#ifdef TC_DYNAMIC_XAMLCPP
#ifdef TC_API_EXPORT_XAMLCPP
#  define TC_XAMLCPP_API TC_API_EXPORT
#else
#  define TC_XAMLCPP_API TC_API_IMPORT
#endif
#else
#  define TC_XAMLCPP_API MODUL_DLL_DECL
#endif

//
#endif //MODUL_DEFINE_H
