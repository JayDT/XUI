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

#include "Reflection/Runtime/IField.h"

#include "Variant.h"

#include "FieldGetter.h"
#include "FieldSetter.h"

namespace System::Reflection
{
    inline namespace meta
    {
        class Type;
        struct FieldDesc;

        class TC_METARUNTIME_API Global : public IField
        {
        public:
            Global(void);

            Global(
                const char* name,
                Type const& type,
                FieldGetterBase *getter, 
                FieldSetterBase *setter, 
                Type const& parentType,
                CXXAccessSpecifier accessLevel = CXXAccessSpecifier::Public
            );

            virtual ~Global();

            static const Global &Invalid(void);

            bool IsStatic(void) const override { return true; }
            bool IsValid(void) const override;
            bool IsReadOnly(void) const override;

            Type const* GetFieldType(void) const override;
            Type const& DeclaringType(void) const override;

            size_t GetFieldTypeHashCode() const override { return m_fieldTypeHashCode; }

            const char* GetName(void) const override;

            CXXAccessSpecifier GetAccessLevel() const override { return AccessLevel; }

            Variant GetValue(void) const;
            Variant GetValueReference() const;
            bool SetValue(const Variant &value) const;

            FieldDesc const* GetFieldInfo() const override { return m_fieldInfo; }

            uint32_t GetUID() const override { return m_UID; }

            // Inherited via IField
            bool InternalSetValue(Variant &/*instance*/, const Variant & value) const override final
            {
                return SetValue(value);
            }

            Variant GetValue(const Object* /*instance*/) const override final
            {
                return GetValue();
            }

            Variant GetValueReference(const Object* instance) const override final
            {
                return GetValueReference();
            }

            virtual Type const * GetElaborateType(void) const override;

            IMethod* GetMethod() const { return m_getter; }
            IMethod* SetMethod() const { return m_setter; }

        private:
            Global(Global const&) = delete;
            Global& operator=(Global const&) = delete;

            friend struct TypeData;

            Type const* m_type;
            Type const* m_parentType;
            const char* m_name;
            FieldDesc const* m_fieldInfo;
            FieldGetterBase* m_getter;
            FieldSetterBase* m_setter;

            uint32_t m_UID;
            size_t m_fieldTypeHashCode;
            CXXAccessSpecifier AccessLevel : 8;


        };
    }
}