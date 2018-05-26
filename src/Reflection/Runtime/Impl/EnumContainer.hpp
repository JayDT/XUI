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

namespace System::Reflection
{
    inline namespace meta
    {
        EnumContainer::EnumContainer(
            const std::string &name, 
            Type const* type,
            Type const* underlyingType,
            const Initializer &initializer
        )
            : EnumBase( name, InvalidTypeID )
            , m_type(type)
            , m_UnderlyingType(underlyingType)
        {
            for (auto &pair : initializer)
            {
                m_keys.emplace_back( pair.first );

                m_keyToValue.emplace( pair );
            }
        }

        ///////////////////////////////////////////////////////////////////////


        EnumContainer::EnumContainer(
            const std::string &name, 
            Type const* type,
            Type const* underlyingType,
            const Initializer &initializer,
            TypeID owner
        )
            : EnumBase( name, owner )
            , m_type(type)
            , m_UnderlyingType(underlyingType)
        {
            for (auto &pair : initializer)
            {
                m_keys.emplace_back( pair.first );

                m_keyToValue.emplace( pair );
            }
        }

        ///////////////////////////////////////////////////////////////////////


        Type const* EnumContainer::GetType(void) const
        {
            return m_type;
        }

        ///////////////////////////////////////////////////////////////////////


        Type const* EnumContainer::GetUnderlyingType(void) const
        {
            return m_UnderlyingType;
        }

        ///////////////////////////////////////////////////////////////////////


        const std::vector<std::string> &EnumContainer::GetKeys(void) const
        {
            return m_keys;
        }

        ///////////////////////////////////////////////////////////////////////


        std::vector<Variant> EnumContainer::GetValues(void) const
        {
            std::vector<Variant> values;

            for (auto &entry : m_keyToValue)
            {
                if (m_type->TypeDescriptor)
                    values.emplace_back(m_type->TypeDescriptor->EnumValue(entry.second));
                else
                    values.emplace_back(Variant(entry.second, ReferenceVariantPolicy));
            }

            return values;
        }

        ///////////////////////////////////////////////////////////////////////


        std::string EnumContainer::GetKey(unsigned long long value) const
        {
            for (auto &entry : m_keyToValue)
            {
                if (entry.second == value)
                    return entry.first;
            }

            return std::string( );
        }

        ///////////////////////////////////////////////////////////////////////


        Variant EnumContainer::GetValue(const std::string &key) const
        {
            auto search = m_keyToValue.find( key );

            // not found
            if (search == m_keyToValue.end( ))
                return Variant( );

            if (m_type->TypeDescriptor)
                return m_type->TypeDescriptor->EnumValue(search->second);
            return Variant(search->second, ReferenceVariantPolicy);
        }
    }
}
