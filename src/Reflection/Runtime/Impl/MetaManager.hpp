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
        template<typename PropertyType>
        const PropertyType *MetaManager::GetProperty(void) const
        {
            static_assert( std::is_base_of<MetaProperty, PropertyType>::value, 
                "Type must be a MetaProperty." 
            );
            
            static const auto& type = typeof( PropertyType );

            if (!m_properties)
                return nullptr;

            std::pair<std::multimap<Type const*, const MetaProperty *>::iterator, std::multimap<Type const*, const MetaProperty *>::iterator> search = m_properties->equal_range(&type);

            // doesn't exist
            if (search.first == search.second)
                return nullptr;

            return static_cast<const PropertyType*>( search.first->second );
        }
    }
}
