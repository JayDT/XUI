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

#include <Reflection/Runtime/Argument.h>
#include <Reflection/Runtime/Type.h>

namespace System::Reflection
{
    inline namespace meta
    {
        template<class... Types>
        struct TypeUnpacker { };

        template<>
        struct TypeUnpacker<>
        {
            static void Apply(Type::List &types) { }
            static void Apply(ArgHashStorage &types) { }
        };

        template<class First, class... Types>
        struct TypeUnpacker<First, Types...>
        {
            static void Apply(Type::List &types)
            {
                typedef typename std::remove_cv<typename std::remove_reference<First>::type>::type CleanType;

                Type const* type = &System::Reflection::type_of(typeid(CleanType));
                if (!type)
                    type = &System::Reflection::type_of(typeid(typename std::remove_pointer<CleanType>::type));
                types.emplace_back(type);

                TypeUnpacker<Types...>::Apply( types );
            }

            static void Apply(ArgHashStorage &types)
            {
                types.emplace_back(typeid(First).hash_code());

                TypeUnpacker<Types...>::Apply(types);
            }

            static void Apply(ArgumentList &args)
            {
                args.emplace_back(typeid(First).hash_code());

                TypeUnpacker<Types...>::Apply(types);
            }
        };

        struct ArgUnpacker
        {
            inline void Apply(ArgumentList &argList, Type::List &types, uint8 index)
            {
            }

            template<class First>
            inline void Apply(ArgumentList &argList, Type::List &types, uint8 index, First&& arg)
            {
                typedef typename std::remove_cv<First>::type CleanType;

                argList.emplace_back(new Argument(CleanType(arg)));
            }

            inline void Apply(ArgumentList &argList, Type::List &types, uint8 index, System::Reflection::Variant const& arg)
            {
                argList.emplace_back(new Argument(arg));
            }

            template<class First, class... Args>
            inline void Apply(ArgumentList &argList, Type::List &types, uint8 index, First&& arg, Args&&... args)
            {
                typedef typename std::remove_cv<First>::type CleanType;

                argList.emplace_back(new Argument(CleanType(arg)));

                Apply(argList, types, index + 1, std::forward<Args>(args)...);
            }

            template<class... Args>
            inline void Apply(ArgumentList &argList, Type::List &types, uint8 index, System::Reflection::Variant const& arg, Args&&... args)
            {
                argList.emplace_back(new Argument(arg));

                Apply(argList, types, index + 1, std::forward<Args>(args)...);
            }

            template<int N>
            inline void Free(ArgumentList &argList)
            {
                delete argList[N];
                Free<int(N - 1)>(argList);
            };

            template<>
            inline void Free<-1>(ArgumentList&)
            {
            };
        };
    }
}