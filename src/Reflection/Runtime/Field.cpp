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

#include "Precompiled.h"

#include "Type.h"
#include "Field.h"
#include "Method.h"
#include "ReflectionModule.h"

namespace System::Reflection
{
    inline namespace meta
    {
        Field::Field(void)
            : m_type( &Type::Invalid( ) )
            , m_classType( &Type::Invalid( ) )
            , m_name( "INVALID" )
            , m_getter( nullptr ) 
            , m_setter( nullptr ) 
            , AccessLevel(CXXAccessSpecifier::Public)
			, m_isStatic(false)
        {
            ++ReflectionModule::sFieldCounter;
        }

        Field::Field(
            const char* name,
            Type const& type,
            Type const& classType,
            FieldGetterBase *getter,
            FieldSetterBase *setter,
            CXXAccessSpecifier accessLevel
        ) 
            : m_type( &type )
            , m_classType( &classType )
            , m_fieldInfo( nullptr )
            , m_name( name )
            , m_getter( getter )
            , m_setter( setter )
            , AccessLevel(accessLevel)
			, m_isStatic(false)
		{
            ++ReflectionModule::sFieldCounter;
            if (m_getter)
                m_getter->m_declaringField = this;
            if (m_setter)
                m_setter->m_declaringField = this;
        }

        Field::~Field()
        {
            --ReflectionModule::sFieldCounter;
        }

        const Field &Field::Invalid(void)
        {
            static Field field;

            return field;
        }

        bool Field::SSetValue(Variant &instance, const Variant &value, const Method &setter)
        {
             // read only?
            if (setter.IsValid())
            {
                setter.IMethod::Invoke( instance, value );

                return true;
            }

            return false;
        }

        bool Field::IsValid(void) const
        {
            return m_getter != nullptr;
        }

        bool Field::IsReadOnly(void) const
        {
            return m_setter == nullptr;
        }

        Type const* Field::GetFieldType(void) const
        {
            return m_type;
        }

        Type const* Field::GetElaborateType(void) const
        {
            return &m_type && m_type->ElaborateType ? m_type->ElaborateType : m_type;
        }

        Type const& Field::DeclaringType(void) const
        {
            return *m_classType;
        }

        const char* Field::GetName(void) const
        {
            return m_name;
        }

        Variant Field::GetValue(const Object* instance) const
        {
            return m_getter->GetValue(m_UID, instance ? instance : System::Reflection::Variant::Empty );
        }

        Variant Field::GetValueReference(const Object* instance) const
        {
            return m_getter->GetValueReference(m_UID, instance ? instance : System::Reflection::Variant::Empty);
        }

        Variant Field::GetInternalValue(const Variant& instance) const
        {
            return m_getter->GetValue(m_UID, instance);
        }

        Variant Field::GetInternalValueReference(const Variant& instance) const
        {
            return m_getter->GetValueReference(m_UID, instance);
        }

        bool Field::InternalSetValue(Variant &instance, const Variant &value) const
        {
            // read only?
            if (m_setter)
            {
                m_setter->SetValue(m_UID, instance, value );

                return true;
            }

            return false;
        }
    }
}
