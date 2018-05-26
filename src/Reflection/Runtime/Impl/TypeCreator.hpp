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

#include "../Variant.h"
#include "../Type.h"
#include "../Constructor.h"

namespace System::Reflection
{
    inline namespace meta
    {
        template<typename ...Args>
        Variant TypeCreator::Create(const Type &type, Args &&...args)
        {
            static Type::List signatureTypes;
            static ArgHashStorage signature;

            static bool initial = true;
            
            if (initial)
            {
                TypeUnpacker<Args...>::Apply( signatureTypes );
                TypeUnpacker<Args...>::Apply( signature );
            
                initial = false;
            }

            Type const* _type = &type;
            if (_type->IsPointer() && _type->ElaborateType)
                _type = type.ElaborateType;

            ArgumentList arguments;
            ArgUnpacker().Apply(arguments, signatureTypes, 0, std::forward<Args>(args)...);

            auto &constructor = _type->GetConstructor(signature);

            auto ret = constructor.InvokeVariadic(arguments);

            ArgUnpacker().Free<int(sizeof...(Args)-1)>(arguments);

            return ret;
        }

        template<typename ...Args>
        static std::shared_ptr<Object> TypeCreator::CreateShared(const Type &type, Args &&...args)
        {
            static Type::List signatureTypes;
            static ArgHashStorage signature;

            static bool initial = true;

            if (initial)
            {
                TypeUnpacker<Args...>::Apply(signatureTypes);
                TypeUnpacker<Args...>::Apply(signature);

                initial = false;
            }

            Type const* _type = &type;
            if (_type->IsPointer() && _type->ElaborateType)
                _type = type.ElaborateType;

            auto &constructor = _type->GetConstructor(signature);
            if (!constructor.IsValid())
                return nullptr;

            ArgumentList arguments;
            ArgUnpacker().Apply(arguments, signatureTypes, 0, std::forward<Args>(args)...);

            auto ret = _type->__class_shared_contructor_impl(constructor.GetUID(), std::move(arguments));

            ArgUnpacker().Free<int(sizeof...(Args)-1)>(arguments);

            return ret;
        }

        template<typename ...Args>
        Variant TypeCreator::CreateDynamic(const Type &type, Args &&...args)
        {
            static Type::List signatureTypes;
            static ArgHashStorage signature;
            
            static bool initial = true;
            
            if (initial)
            {
                TypeUnpacker<Args...>::Apply(signatureTypes);
                TypeUnpacker<Args...>::Apply( signature );
            
                initial = false;
            }

            Type const* _type = &type;
            if (_type->IsPointer() && _type->ElaborateType)
                _type = type.ElaborateType;

            ArgumentList arguments;
            ArgUnpacker().Apply(arguments, signatureTypes, 0, std::forward<Args>(args)...);

            auto &constructor = _type->GetDynamicConstructor( signature );

            auto ret = constructor.InvokeVariadic( arguments );

            ArgUnpacker().Free<int(sizeof...(Args) - 1)>( arguments );

            return ret;
        }

    }
}