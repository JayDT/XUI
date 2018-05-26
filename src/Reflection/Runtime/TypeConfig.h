/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 Austin Brunkhorst, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "standard/Platform/Define.h"

#include "TypeID.h"
#include "MetaTraits.h"

#include <type_traits>
#include <typeinfo>


#undef typeidof
#undef typeof

namespace System::Reflection
{
    inline namespace meta
    {
        class TC_METARUNTIME_API Type;

        TC_METARUNTIME_API Type const& ManagedTypeOf(TypeID typeID);
        TC_METARUNTIME_API Type const& ManagedTypeOf(std::type_info const& rhs);
        TC_METARUNTIME_API Type const& ManagedTypeOf(std::size_t rhs);
        TC_METARUNTIME_API TypeID const& ManagedTypeID(std::type_info const& rhs);
        TC_METARUNTIME_API TypeID const& ManagedTypeID(size_t rhs);
    }

    inline TC_METARUNTIME_API meta::TypeID const& typeid_of(std::type_info const& typeinfo)
    {
        return System::Reflection::meta::ManagedTypeID(typeinfo);
    }

    template<typename T>
    inline meta::TypeID const& typeid_of()
    {
        return System::Reflection::meta::ManagedTypeID(typeid(T));
    }

    template<typename T>
    inline meta::Type const& type_of()
    {
        return System::Reflection::meta::ManagedTypeOf(typeid_of<T>());
    }

    inline TC_METARUNTIME_API meta::Type const& type_of(meta::TypeID type_id)
    {
        return System::Reflection::meta::ManagedTypeOf(type_id);
    }

    inline TC_METARUNTIME_API meta::Type const& type_of(std::type_info const& typeinfo)
    {
        return System::Reflection::meta::ManagedTypeOf(System::Reflection::meta::ManagedTypeID(typeinfo));
    }

    inline TC_METARUNTIME_API meta::Type const& type_of(size_t nativetypeid)
    {
        return System::Reflection::meta::ManagedTypeOf(System::Reflection::meta::ManagedTypeID(nativetypeid));
    }

    template<typename TType>
    inline const System::Reflection::Type& CacheType()
    {
        static const System::Reflection::Type& type = System::Reflection::meta::ManagedTypeOf(typeid(System::Reflection::meta::CleanedType<System::Reflection::meta_traits::RemoveArray<TType>::type>)); //typeof(TType);
        return type;
    }
}

// Gets the type ID of a given expression (generate too many object)
//#define typeidof(expr)                                                          \
//    System::Reflection::meta::TypeInfo<                                         \
//        System::Reflection::meta::CleanedType<                                  \
//            typename System::Reflection::meta_traits::RemoveArray<expr>::type   \
//        >                                                                       \
//    >::ID                                                                       \

// Converts the expression into a meta::Type instance
//#define typeof(expr)                              \
//    System::Reflection::meta::Type(                           \
//        typeidof( expr ),                         \
//        System::Reflection::meta_traits::IsArray<expr>::value \
//    )                                             \

// System::Reflection::meta::TypeInfo<System::Reflection::meta::CleanedType<typename System::Reflection::meta_traits::RemoveArray<expr>::type>>::ID 
#define typeidof(...) System::Reflection::meta::ManagedTypeID(typeid(System::Reflection::meta::CleanedType<System::Reflection::meta_traits::RemoveArray<__VA_ARGS__>::type>))
// (*System::Reflection::meta::TypeInfo<System::Reflection::meta::CleanedType<typename System::Reflection::meta_traits::RemoveArray<expr>::type>>::TypeDesc) 
#define typeof(...) System::Reflection::CacheType<System::Reflection::meta::CleanedType<System::Reflection::meta_traits::RemoveArray<__VA_ARGS__>::type>>()
//System::Reflection::meta::ManagedTypeOf(typeid(System::Reflection::meta::CleanedType<System::Reflection::meta_traits::RemoveArray<__VA_ARGS__>::type>))
#define typeinstanceidof(expr) System::Reflection::meta::ManagedTypeID(typeid(*expr))
#define typeinstanceof(expr) System::Reflection::meta::ManagedTypeOf(typeid(*expr))

#define _template_typeidof(...) System::Reflection::meta::ManagedTypeID(typeid(typename System::Reflection::meta::CleanedType<typename System::Reflection::meta_traits::RemoveArray<__VA_ARGS__>::type>))
#define _template_typeof(...) System::Reflection::meta::ManagedTypeOf(typeid(typename System::Reflection::meta::CleanedType<typename System::Reflection::meta_traits::RemoveArray<__VA_ARGS__>::type>))

#define typeofbytypeid(...) System::Reflection::meta::ManagedTypeOf(__VA_ARGS__)

// Converts the resulting type of the given expression to a meta::Type instance
#define decltypeof(expr) typeof( decltype( expr ) )

namespace System::Reflection
{
    inline namespace meta
    {
        template<typename T>
        using CleanedType = 
                typename System::Reflection::meta_traits::RemoveSharedPtr<
                    typename std::remove_cv< 
                        typename std::remove_reference< T >::type 
                    >::type
                >::type;
    }
}