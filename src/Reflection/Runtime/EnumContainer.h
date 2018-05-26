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

#include "EnumBase.h"
#include "Variant.h"

namespace System::Reflection
{
    inline namespace meta
    {
        class EnumContainer : public EnumBase
        {
        public:
            typedef std::initializer_list<std::pair<std::string, unsigned long long>> Initializer;
            typedef std::map<std::string, unsigned long long> Table;

            EnumContainer(
                const std::string &name, 
                Type const* type,
                Type const* underlyingType,
                const Initializer &initializer);
            
            EnumContainer(
                const std::string &name, 
                Type const* type,
                Type const* underlyingType,
                const Initializer &initializer,
                TypeID owner
            );

            Type const* GetType(void) const override;
            Type const* GetUnderlyingType(void) const override;

            const std::vector<std::string> &GetKeys(void) const override;
            std::vector<Variant> GetValues(void) const override;

            std::string GetKey(unsigned long long value) const override;
            Variant GetValue(const std::string &key) const override;

        private:
            Table m_keyToValue;
            std::vector<std::string> m_keys;
            Type const* m_type;
            Type const* m_UnderlyingType;
        };
    }
}

#include "Impl/EnumContainer.hpp"