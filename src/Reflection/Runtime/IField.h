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

#include "Reflection/Runtime/IMemberInfo.h"

namespace System::Reflection
{
    inline namespace meta
    {
        struct FieldDesc;
        struct IMethod;
        class Variant;

        struct TC_METARUNTIME_API IField : IMemberInfo
        {
            virtual ~IField() {}

            virtual bool InternalSetValue(Variant &instance, const Variant &value) const = 0;
            virtual Variant GetValue(const Object *instance) const = 0;
            virtual Variant GetValueReference(const Object* instance) const = 0;

            virtual FieldDesc const* GetFieldInfo() const = 0;

            virtual bool IsProperty(void) const { return false; }
            virtual bool IsStatic(void) const = 0;
            virtual bool IsValid(void) const = 0;
            virtual bool IsReadOnly(void) const = 0;

            virtual size_t GetFieldTypeHashCode() const = 0;
            virtual Type const* GetFieldType(void) const = 0;
            virtual Type const* GetElaborateType(void) const = 0;

            virtual IMethod* GetMethod() const = 0;
            virtual IMethod* SetMethod() const = 0;

            virtual bool CanWrite() const { return SetMethod() != nullptr ? true : false; }
            virtual bool CanRead() const { return GetMethod() != nullptr ? true : false; }

            template<typename ARG>
            bool SetValue(Object const* instance, ARG value) const;

            bool SetValue(Object const* instance, Variant const& value) const
            {
                Variant _instance = Variant(instance, ReferenceVariantPolicy);
                return InternalSetValue(_instance, value);
            }
        };

        template<typename ARG>
        inline bool IField::SetValue(Object const * instance, ARG value) const
        {
            //typedef typename std::remove_cv<typename std::remove_reference<ARG>::type>::type CleanValueType;

            Variant _instance = Variant(instance, ReferenceVariantPolicy);
            Variant _value = Variant(value, ReferenceVariantPolicy);
            return InternalSetValue(_instance, _value);
        }
    }
}