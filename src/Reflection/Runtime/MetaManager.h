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

#include <string>
#include <map>
#include <vector>

#include "JsonConfig.h"
#include "Variant.h"

namespace System::Reflection
{
    inline namespace meta
    {
        class Type;
        class MetaProperty;

        // From Clang header
        /**
        * \brief Represents the C++ access control level to a base class for a
        * cursor with kind CX_CXXBaseSpecifier.
        */
        enum class CXXAccessSpecifier
        {
            InvalidAccessSpecifier,
            Public,
            Protected,
            Private
        };

        // From Clang header
        enum CXVisibilityKind
        {
            /** \brief This value indicates that no visibility information is available
            * for a provided CXCursor. */
            CXVisibility_Invalid,

            /** \brief Symbol not seen by the linker. */
            CXVisibility_Hidden,
            /** \brief Symbol seen by the linker but resolves to a symbol inside this object. */
            CXVisibility_Protected,
            /** \brief Symbol seen by the linker and acts like a normal symbol. */
            CXVisibility_Default
        };

        class TC_METARUNTIME_API MetaManager
        {
        public:
            typedef std::initializer_list<
                std::pair<Type const*, const MetaProperty *>
            > Initializer;

            typedef std::vector<Variant> PropertyList;

            MetaManager(void);
            MetaManager(const MetaManager &rhs);
            MetaManager(const MetaManager &&rhs);

            const MetaManager &operator=(const MetaManager &rhs);

            MetaManager(const Initializer &properties);

            void Append(const Initializer &meta);

            ~MetaManager(void);

            Variant GetProperty(Type const& type) const;

            template<typename PropertyType>
            const PropertyType *GetProperty(void) const;

            // Sets the given property
            void SetProperty(Type const& type, const MetaProperty *value);

            // Gets all properties
            PropertyList GetProperties(void) const;

            json11::Json SerializeJson(void) const;

            template<typename TAttr>
            const TAttr* GetCustomAttribute() const
            {
                return GetProperty<TAttr>();
            }

            template<typename TAttr>
            std::vector<const TAttr*> GetCustomAttributes() const
            {
                static_assert(std::is_base_of<MetaProperty, TAttr>::value,
                    "Type must be a MetaProperty."
                    );

                static const auto& type = typeof(TAttr);

                std::vector<const TAttr*> result;
                if (m_properties)
                {
                    std::pair<std::multimap<Type const*, const MetaProperty *>::iterator, std::multimap<Type const*, const MetaProperty *>::iterator> search = m_properties->equal_range(&type);
                    for (auto itr = search.first; itr != search.second; ++itr)
                        result.push_back(static_cast<const TAttr*>(itr->second));
                }
                return result;;
            }

        private:
            void clear();
            void copy(const MetaManager &rhs);

            std::multimap<Type const*, const MetaProperty *>* m_properties = nullptr;
        };
    }
}

#include "Impl/MetaManager.hpp"