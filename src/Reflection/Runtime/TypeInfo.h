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

#include "TypeConfig.h"

#include "Common/Compiler.h"

#include <type_traits>

#ifndef WIN32

#define IsTriviallyDefaultConstructible(x) std::has_trivial_default_constructor<x>::value

#else

#define IsTriviallyDefaultConstructible(x) std::is_trivially_default_constructible<x>::value

#endif

#define RegisterReflType(id, Template, _type, _ElaborateType)                                                                               \
if (id != System::Reflection::meta::InvalidTypeID)                                                                                          \
{                                                                                                                                           \
    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = _ElaborateType;                                                     \
    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                                                       \
                                                                                                                                            \
    Data->isTrivialConstruct = std::is_trivially_default_constructible<Template>::value;                                                    \
    Data->isClass = std::is_class< std::remove_pointer< std::decay<Template>::type >::type >::value;                                        \
    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<Template>::type >::type >::value;                                          \
    Data->isPointer = std::is_pointer< Template >::value;                                                                                   \
    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<Template>::type >::type >::value;                               \
    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<Template>::type >::type >::value;                       \
    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<Template>::type >::type >::value;                                      \
    Data->size = sizeof(_Get_size_type<Template>::type);                                                                                    \
    Data->native_hash_code = typeid(Template).hash_code();                                                                                  \
    Data->TypeDescriptor = new System::Reflection::TypeDescriptor<Template>;                                                                \
}                                                                                                                                           \

template<class _Ty>
struct _Get_size_type
{	// get _Ty::size_type
    typedef _Ty type;
};

template<>
struct _Get_size_type<void>
{	// provide fallback
    typedef void* type;
};


namespace System::Reflection
{
    inline namespace meta
    {
    }
}

#include "Impl/TypeInfo.hpp"
