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

#include "Reflection/Runtime/IField.h"

#include "Variant.h"

#include "FieldGetter.h"
#include "FieldSetter.h"

namespace System::Reflection
{
    inline namespace meta
    {
        class Method;
        struct FieldDesc;

        class TC_METARUNTIME_API Field : public IField
        {
        public:
            static bool SSetValue(Variant &instance, const Variant &value, const Method &setter);

            typedef std::vector<Field*> List;

            Field(void);

            Field(
                const char* name,
                Type const& type, 
                Type const& classType, 
                FieldGetterBase *getter, 
                FieldSetterBase *setter,
                CXXAccessSpecifier accessLevel = CXXAccessSpecifier::Public
            );

            virtual ~Field();

            static const Field &Invalid(void);

            bool IsProperty(void) const override final { return m_isProperty; }
            bool IsStatic(void) const override final { return m_isStatic; }
            bool IsValid(void) const override final;
            bool IsReadOnly(void) const override final;

            size_t GetFieldTypeHashCode() const override final { return m_fieldTypeHashCode; }
            Type const* GetFieldType(void) const override final;
            Type const* GetElaborateType(void) const override final;
            Type const& DeclaringType(void) const override final;

            const char* GetName(void) const override final;

            CXXAccessSpecifier GetAccessLevel() const override final { return AccessLevel; }

            Variant GetValue(const Object* instance) const override final;
            Variant GetValueReference(const Object* instance) const override final;
            bool InternalSetValue(Variant &instance, const Variant &value) const override final;
            Variant GetInternalValue(const Variant& instance) const;
            Variant GetInternalValueReference(const Variant& instance) const;

            FieldDesc const* GetFieldInfo() const override final { return m_fieldInfo; }

            uint32_t GetUID() const override final { return m_UID; }

            IMethod* GetMethod() const { return m_getter; }
            IMethod* SetMethod() const { return m_setter; }

        private:
            Field(Field const&) = delete;
            Field& operator=(Field const&) = delete;

            friend struct TypeData;

            Type const* m_type;
            Type const* m_classType;

            FieldDesc const* m_fieldInfo;
            const char* m_name;
            FieldGetterBase* m_getter;
            FieldSetterBase* m_setter;

            size_t m_fieldTypeHashCode;
            uint32_t m_UID;
            CXXAccessSpecifier AccessLevel : 8;
            bool m_isProperty : 1;
			bool m_isStatic : 1;
        };
    }
}