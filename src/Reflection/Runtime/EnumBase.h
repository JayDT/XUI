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

#include "TypeID.h"
#include "Variant.h"

#include <string>
#include <vector>

namespace System::Reflection
{
    inline namespace meta
    {
        class Type;
        class Argument;

        class TC_METARUNTIME_API EnumBase
        {
        public:
            EnumBase(const std::string &name, TypeID owner);

            virtual ~EnumBase(void) { }

            virtual Type const* GetType(void) const = 0;
            virtual Type const* GetUnderlyingType(void) const = 0;

            virtual const std::vector<std::string> &GetKeys(void) const = 0;
            virtual std::vector<Variant> GetValues(void) const = 0;

            virtual std::string GetKey(unsigned long long value) const = 0;
            virtual Variant GetValue(const std::string &key) const = 0;

            Type const* GetParentType(void) const;

            const std::string &GetName(void) const;

        private:
            Type const& m_parentType;

            std::string m_name;
        };
    }
}