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

#include <standard/Platform/Define.h>
#include <vector>
#include <map>

// number of arguments within the usage of a dynamic method/function invoker
#define THIS_ARG_COUNT sizeof...( ArgTypes )

namespace System::Reflection
{
    inline namespace meta
    {
        class Type;

        typedef std::vector<std::size_t> ArgHashStorage;
        typedef std::vector<Type const*> InvokableSignature;

        template<typename T>
        using InvokableOverloadMap = 
            std::multimap<InvokableSignature, T>;

        // maximum number of arguments supported
        //const size_t MaxArgumentCount = 10;

        struct TC_METARUNTIME_API FieldDesc
        {
            enum class FieldType
            {
                NONE = 0,
                FIELD,
                PROPERTY,
                STATIC_FIELD,
            };

            FieldDesc() {};
            FieldDesc(uint32 id, Type const& ct, Type const& rt, FieldType methodType, size_t offset)
                : m_signatureId(id)
                , m_classType(&ct)
                , m_fieldType(&rt)
                , m_fieldMode(methodType)
                , m_offset(offset)
            {}

            uint32 m_signatureId = 0;
            Type const* m_classType = nullptr;
            Type const* m_fieldType = nullptr;
            FieldType m_fieldMode;
            size_t m_offset = 0;
        };

        struct TC_METARUNTIME_API MethodDesc
        {
            enum class MethodType
            {
                NONE = 0,
                CLASS_METHOD,
                STATIC_METHOD,
                GLOBAL_METHOD,
                CONSTRUCTOR,
                DYNAMIC_CONSTRUCTOR,
                ARRAY_CONSTRUCTOR,
                DESTRUCTOR
            };

            MethodDesc() {}
            MethodDesc(uint32 id, Type const& ct, Type const& rt, MethodType methodType, ArgHashStorage const& args)
                : m_signatureId(id)
                , m_classType(&ct)
                , m_returnType(&rt)
                , m_methodType(methodType)
                , m_argHashStorage(std::move(args))
            {}

            uint32 m_signatureId = 0;
            Type const* m_classType = nullptr;
            Type const* m_returnType = nullptr;
            MethodType m_methodType;
            ArgHashStorage m_argHashStorage;

            InvokableSignature& GetInvokableSignature() const;

        private:
            mutable InvokableSignature m_invokeSignatures;
        };
    }
}