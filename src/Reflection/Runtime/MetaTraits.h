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

#include "Array.h"
#include <memory>

namespace System::Reflection
{
    inline namespace meta_traits
    {
        template<typename T, typename = void>
        struct TypeOrEnumType
        {
            typedef T type;
        };

        template<typename T>
        struct TypeOrEnumType<T,
            typename std::enable_if<
            std::is_enum<T>::value
            >::type
        >
        {
            typedef typename std::underlying_type<T>::type type;
        };

        ///////////////////////////////////////////////////////////////////////////

        template<typename T>
        struct RemoveArray
        {
            typedef T type;
        };

        template<typename T>
        struct RemoveArray<Array<T>>
        {
            typedef T type;
        };

        ///////////////////////////////////////////////////////////////////////////

        template<typename T>
        struct RemoveSharedPtr
        {
            typedef T type;
        };

        template<typename T>
        struct RemoveSharedPtr<std::shared_ptr<T>>
        {
            typedef T type;
        };
        ///////////////////////////////////////////////////////////////////////////

        template<typename T>
        struct IsArray
        {
            static const bool value = false;
        };

        template<typename T>
        struct IsArray<Array<T>>
        {
            static const bool value = true;
        };

        ///////////////////////////////////////////////////////////////////////////

        template<typename T>
        using ArrayByValue = Array<T>;

        template<typename T>
        using ArrayByReference = typename std::add_lvalue_reference<Array<T>>::type;
    }
}
