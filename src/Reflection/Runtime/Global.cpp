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
#include "Global.h"
#include "Argument.h"
#include "ReflectionModule.h"

namespace System::Reflection
{
    inline namespace meta
    {
        Global::Global(void)
            : m_type( &Type::Invalid( ) )
            , m_parentType( &Type::Invalid( ) )
            , m_name( "INVALID" )
            , m_getter( nullptr ) 
            , m_setter( nullptr )
            , AccessLevel(CXXAccessSpecifier::Public)
        {
            ++ReflectionModule::sFieldCounter;
        }

        Global::Global(
            const char* name,
            Type const& type,
            FieldGetterBase *getter,
            FieldSetterBase *setter,
            Type const& parentType,
            CXXAccessSpecifier accessLevel
        ) 
            : m_type( &type )
            , m_parentType( &parentType ) 
            , m_name( name ) 
            , m_getter( getter )
            , m_setter( setter )
            , AccessLevel(accessLevel) 
        {
            ++ReflectionModule::sFieldCounter;

            if (m_getter)
                m_getter->m_declaringField = this;
            if (m_setter)
                m_setter->m_declaringField = this;
        }

        Global::~Global()
        {
            --ReflectionModule::sFieldCounter;
        }

        const Global &Global::Invalid(void)
        {
            static Global global;

            return global;
        }

        bool Global::IsValid(void) const
        {
            return m_getter != nullptr;
        }

        bool Global::IsReadOnly(void) const
        {
            return m_setter == nullptr;
        }

        Type const* Global::GetFieldType(void) const
        {
            return m_type;
        }

        Type const * Global::GetElaborateType(void) const
        {
            return &m_type && m_type->ElaborateType ? m_type->ElaborateType : m_type;;
        }

        Type const& Global::DeclaringType(void) const
        {
            return *m_parentType;
        }

        const char* Global::GetName(void) const
        {
            return m_name;
        }

        Variant Global::GetValue(void) const
        {
            return m_getter->GetValue(m_UID, Variant::Empty);
        }

        Variant Global::GetValueReference() const
        {
            return m_getter->GetValueReference(m_UID, Variant::Empty);
        }

        bool Global::SetValue(const Variant &value) const
        {
            // read only?
            if (m_setter != nullptr)
            {
                m_setter->SetValue(m_UID, Variant::Empty, value );

                return true;
            }
               
            return false;
        }
    }
}
