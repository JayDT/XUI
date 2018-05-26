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

#ifndef _COMMON_H__
#define _COMMON_H__

// WorkAround for vs 15.5
#if defined(__REFLECTION_PARSER__)
# define __is_trivially_destructible(...) false
#endif

#include <queue>
#include <memory>
#include <boost/cstdint.hpp>
#include <string.h>
#include <assert.h>

#if defined(__sun__)
#include <ieeefp.h> // finite() on Solaris
#endif

#include <set>
#include <vector>
#include <list>
#include <string>
#include <map>
#include <mutex>
#include <shared_mutex>
#include "sparsehash/dense_hash_map"

#include "standard/Platform/Define.h"
#include "standard/Preprocessor/pp_platform.h"

#if COMPILER == COMPILER_MICROSOFT
#   ifndef __SHOW_STUPID_WARNINGS__
#       pragma warning(disable:4267)                             // 'identifier' : macro redefinition
#       pragma warning(disable:4985)                             // 'identifier' : macro redefinition
#       pragma warning(disable:4996)                             // 'identifier' : macro redefinition
#       pragma warning(disable:4005)                             // 'identifier' : macro redefinition
#       pragma warning(disable:4018)                             // 'expression' : signed/unsigned mismatch
#       pragma warning(disable:4244)                             // 'argument' : conversion from 'type1' to 'type2', possible loss of data
#       pragma warning(disable:4267)                             // 'var' : conversion from 'size_t' to 'type', possible loss of data
#       pragma warning(disable:4305)                             // 'identifier' : truncation from 'type1' to 'type2'
#       pragma warning(disable:4311)                             // 'variable' : pointer truncation from 'type' to 'type'
#       pragma warning(disable:4355)                             // 'this' : used in base member initializer list
#       pragma warning(disable:4800)                             // 'type' : forcing value to bool 'true' or 'false' (performance warning)
#       pragma warning(disable:4275)                             // non dll-interface class 'type' used as base for dll-interface class 'type'
#   endif                                                      // __SHOW_STUPID_WARNINGS__
#   pragma warning (error: 4477)                                 // 'printf' : format string '%s' requires an argument of type 'char *', but variadic argument 1 has type 'std::string'
#else
#   ifdef __INTEL_COMPILER
#       pragma warning disable 68
#       pragma warning disable 654
#       pragma warning disable 858
#       pragma warning disable 279 // constant assert
#       pragma warning disable 858 // type qualifier on return type is meaningless
#   endif
#endif                                                      // __GNUC__

#include "standard/UnorderedMapSet.h"

#include "standard/BasicPrimities.h"
#include "standard/misc.h"
#include "System/rtString.h"

#if PLATFORM == PLATFORM_WINDOWS
#  if defined(__INTEL_COMPILER)
#    if !defined(BOOST_ASIO_HAS_MOVE)
#      define BOOST_ASIO_HAS_MOVE
#    endif // !defined(BOOST_ASIO_HAS_MOVE)
#  endif // if defined(__INTEL_COMPILER)
#else
#  include <sys/types.h>
#  include <sys/ioctl.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <unistd.h>
#  include <netdb.h>
#endif

#if COMPILER == COMPILER_MICROSOFT

#include <float.h>

#define snprintf _snprintf
#define atoll _atoi64
#define vsnprintf _vsnprintf
#define llabs _abs64
#define I32FMT "%08I32X"
#define I64FMT "%016I64X"
#define snprintf _snprintf
#define vsnprintf _vsnprintf

inline int finite(double x) { return _finite(x); }

#else

#define stricmp strcasecmp
#define strnicmp strncasecmp

#  define stricmp strcasecmp
#  define strnicmp strncasecmp

#  define I32FMT "%08X"
#  if ACE_SIZEOF_LONG == 8
#    define I64FMT "%016lX"
#  else
#    define I64FMT "%016llX"
#  endif /* ACE_SIZEOF_LONG == 8 */

#endif

#define INT64_FORMAT_SPECIFIER_ASCII "%I64d"
#define UINT64_FORMAT_SPECIFIER_ASCII "%I64u"

#ifdef WIN32
#define UI64FMTD "%I64u"
#define SI64FMTD "%I64d"
#else
#include <inttypes.h>
#define UI64FMTD "%" PRIu64
#define SI64FMTD "%" PRId64
#endif

#define UI64LIT(N) N ## ull

#define SI64LIT(N) N ## ll

#define SIZEFMTD "%Id"

#define SZFMTD SIZEFMTD

inline float finiteAlways(float f) { return finite(f) ? f : 0.0f; }
inline unsigned long atoul(char const* str) { return strtoul(str, nullptr, 10); }
inline unsigned long long atoull(char const* str) { return strtoull(str, nullptr, 10); }

#define atol(a) strtoul( a, NULL, 10)

#define STRINGIZE(a) #a

// used for creating values for respawn for example
#define MAKE_PAIR64(l, h)  uint64( uint32(l) | ( uint64(h) << 32 ) )
#define PAIR64_HIPART(x)   (uint32)((uint64(x) >> 32) & UI64LIT(0x00000000FFFFFFFF))
#define PAIR64_LOPART(x)   (uint32)(uint64(x)         & UI64LIT(0x00000000FFFFFFFF))

#define MAKE_PAIR16(l, h)  uint16(uint8(l) | (uint16(h) << 8))
#define MAKE_PAIR32(l, h)  uint32( uint16(l) | ( uint32(h) << 16 ) )
#define PAIR32_HIPART(x)   (uint16)((uint32(x) >> 16) & 0x0000FFFF)
#define PAIR32_LOPART(x)   (uint16)(uint32(x)         & 0x0000FFFF)
#ifndef WIN32
#define LOBYTE(w)           ((uint8)(((uint16)(w)) & 0xff))
#define HIBYTE(w)           ((uint8)((((uint16)(w)) >> 8) & 0xff))
#endif

#ifndef _STRINGIZE
#define _STRINGIZE_DETAIL(x) #x
#define _STRINGIZE(x) _STRINGIZE_DETAIL(x)
#endif
#define __LOCATION__ __FILE__ " : " _STRINGIZE(__LINE__)

#ifdef WIN32
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
#include <time.h>

inline struct tm* localtime_r(const time_t* time, struct tm *result)
{
    localtime_s(result, time);
    return result;
}
#endif

#endif
