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
#include "Enum.h"
#include "ReflectionModule.h"

#include "Variant.h"

namespace System::Reflection
{
    inline namespace meta
    {
        Enum::Enum(const EnumBase *base)
            : m_base( base )
        {
            ++ReflectionModule::sEnumCounter;
        }

        Enum::~Enum()
        {
            delete m_base;
            --ReflectionModule::sEnumCounter;
        }

        bool Enum::IsValid(void) const
        {
            return m_base != nullptr;
        }

        Enum::operator bool(void) const
        {
            return m_base != nullptr;
        }

        bool Enum::operator==(const Enum &rhs) const
        {
            return m_base == rhs.m_base;
        }

        bool Enum::operator!=(const Enum &rhs) const
        {
            return m_base != rhs.m_base;
        }

        std::string Enum::GetName(void) const
        {
            return m_base ? m_base->GetName( ) : std::string( );
        }

        Type const* Enum::GetType(void) const
        {
            if (m_base)
                return m_base->GetType();
            return &Type::Invalid();
        }

        Type const* Enum::GetParentType(void) const
        {
            if (m_base)
                return m_base->GetParentType( );
            return &Type::Invalid();
        }

        Type const* Enum::GetUnderlyingType(void) const
        {
            if (m_base)
                return m_base->GetUnderlyingType();
            return &Type::Invalid();
        }

        std::vector<std::string> Enum::GetKeys(void) const
        {
            return m_base ? m_base->GetKeys( ) : std::vector<std::string>( );
        }

        std::vector<Variant> Enum::GetValues(void) const
        {
            return m_base ? m_base->GetValues( ) : std::vector<Variant>( );
        }

        std::string Enum::GetKey(unsigned long long value) const
        {
            return m_base ? m_base->GetKey( value ) : std::string( );
        }

        Variant Enum::GetValue(const std::string &key) const
        {
            return m_base ? m_base->GetValue( key ) : Variant( );
        }

        Variant Enum::Parse(Type const& type, std::string const& value)
        {
            return type.GetEnum()->GetValue(value);
        }
        std::string Enum::GetKey(Type const & type, unsigned long long value)
        {
            if (type.GetEnum())
                return std::move(type.GetEnum()->GetKey(value));
            return "";
        }
        std::vector<std::string> Enum::GetKeys(Type const & type)
        {
            if (type.GetEnum())
                return std::move(type.GetEnum()->GetKeys());
            return std::vector<std::string>();
        }
        Variant Enum::GetValue(Type const & type, std::string const & key)
        {
            if (type.GetEnum())
                return type.GetEnum()->GetValue(key);
            return Variant::Empty;
        }
        std::vector<Variant> Enum::GetValues(Type const & type)
        {
            if (type.GetEnum())
                return std::move(type.GetEnum()->GetValues());
            return std::vector<Variant>();
        }
    }
}
