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

#ifndef MANGOS_UNORDERED_MAP_H
#define MANGOS_UNORDERED_MAP_H

#  include <unordered_map>
#  include <unordered_set>

#define UNORDERED_MAP std::unordered_map
#define UNORDERED_SET std::unordered_set
#define HASH_NAMESPACE_START namespace std {
#define HASH_NAMESPACE_END }

#if COMPILER != COMPILER_MICROSOFT

// Visual Studio use non standard hash calculation function, so provide fake forward for other
HASH_NAMESPACE_START

template<class K>
size_t hash_value(K const&);

HASH_NAMESPACE_END

#endif

#if COMPILER == COMPILER_MICROSOFT && _MSC_VER >= 1700 && _BUILD_PLATFORM_TOOL_SET > 100    // VS110
#define STANDARD_HASH(TYPE, COMPARE, METHOD) \
template<> \
class std::hash<TYPE> \
{ \
    public: \
     \
        size_t operator() (TYPE const& key) const \
        { \
            return std::hash<COMPARE>()(key. METHOD); \
        } \
}; \
     \
HASH_NAMESPACE_START \
     \
 \
    inline size_t hash_value(TYPE const& key) \
    { \
        return hash_value(key. METHOD); \
    } \
     \
HASH_NAMESPACE_END

#else

#define STANDARD_HASH(TYPE, COMPARE, METHOD) \
HASH_NAMESPACE_START \
     \
    template<> \
    class hash<TYPE> \
    { \
    public: \
     \
        size_t operator() (TYPE const& key) const \
        { \
            return hash<COMPARE>()(key. METHOD); \
        } \
    }; \
     \
    inline size_t hash_value(TYPE const& key) \
    { \
        return hash_value(key. METHOD); \
    } \
     \
HASH_NAMESPACE_END
#endif

#endif
