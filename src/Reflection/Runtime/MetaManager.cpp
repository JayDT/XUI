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
#include "MetaManager.h"

#include "Variant.h"
#include "ReflectionDatabase.h"
#include "Reflection/Runtime/ReflectionModule.h"
#include "MetaProperty.h"

#include "Common/Logging.h"

#include <algorithm>

namespace System::Reflection
{
    using Json = json11::Json;
}

namespace System::Reflection
{
    inline namespace meta
    {
        MetaManager::MetaManager(void) : m_properties(nullptr) { }

        MetaManager::MetaManager(const MetaManager &rhs)
            : m_properties(rhs.m_properties)
        {
            const_cast<MetaManager &>(rhs).m_properties = nullptr;
        }

        MetaManager::MetaManager(const MetaManager &&rhs)
            : m_properties(rhs.m_properties)
        {
            const_cast<MetaManager &>(rhs).m_properties = nullptr;
        }

        const MetaManager &MetaManager::operator=(const MetaManager &rhs)
        {
            copy( rhs );

            return *this;
        }

        ///////////////////////////////////////////////////////////////////////

        MetaManager::MetaManager(const Initializer &properties)
        {
            Append(properties);
        }

        void MetaManager::Append(const MetaManager::Initializer & properties)
        {
            for (auto &prop : properties)
            {
                UAssert(prop.first && prop.first->IsValid(),
                    "Invalid meta property registered."
                    "Make sure the property type is enabled in reflection."
                );

                SetProperty(*prop.first, prop.second);
            }
        }

        MetaManager::~MetaManager(void)
        {
            clear();
        }

        ///////////////////////////////////////////////////////////////////////

        Variant MetaManager::GetProperty(Type const& type) const
        {
            if (m_properties)
            {
                auto search = m_properties->find(&type);

                if (search == m_properties->end())
                    return { };

                return Variant(search->second, static_cast<variant_policy::NoCopy*>(nullptr));
            }

            return { };
        }

        ///////////////////////////////////////////////////////////////////////

        void MetaManager::SetProperty(Type const& type, const MetaProperty *prop)
        {
            if (!m_properties)
                m_properties = new std::multimap<Type const*, const MetaProperty *>;

            (*m_properties).insert(std::make_pair(&type, prop));
        }

        MetaManager::PropertyList MetaManager::GetProperties(void) const
        {
            PropertyList properties;

            if (m_properties)
                for (auto &property : *m_properties)
                    properties.emplace_back( Variant( property.second , static_cast<variant_policy::NoCopy*>(nullptr)) );

            return properties;
        }

        Json MetaManager::SerializeJson(void) const
        {
            Json::object object { };

            if (m_properties)
            {
                for (auto &property : *m_properties)
                {
                    auto instance = Variant(property.second, static_cast<variant_policy::NoCopy*>(nullptr));

                    auto const& type = instance.GetType();

                    object[type->GetName()] = type->SerializeJson(instance);
                }
            }

            return object;
        }

        void MetaManager::clear()
        {
            if (m_properties)
            {
                for (auto &prop : *m_properties)
                    delete prop.second;

                delete m_properties;
                m_properties = nullptr;
            }
        }

        void MetaManager::copy(const MetaManager &rhs)
        {
            if (rhs.m_properties)
            {
                if (!m_properties)
                    m_properties = new std::multimap<Type const*, const MetaProperty *>;
                m_properties = rhs.m_properties;
                const_cast<MetaManager &>(rhs).m_properties = nullptr;
            }
            else
            {
                clear();
            }

            //for (auto &prop : rhs.m_properties) 
            //{
            //    m_properties[ prop.first ] = 
            //        (MetaProperty*)(prop.second->Clone());
            //}
        }
    }
}
