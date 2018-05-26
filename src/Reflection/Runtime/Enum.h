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

namespace System::Reflection
{
    inline namespace meta
    {
        class Type;

        class TC_METARUNTIME_API Enum
        {
        public:
            virtual ~Enum();

            bool IsValid(void) const;

            operator bool(void) const;

            bool operator ==(const Enum &rhs) const;
            bool operator !=(const Enum &rhs) const;

            std::string GetName(void) const;

            Type const* GetType(void) const;
            Type const* GetParentType(void) const;
            Type const* GetUnderlyingType(void) const;

            std::vector<std::string> GetKeys(void) const;
            std::vector<Variant> GetValues(void) const;

            std::string GetKey(unsigned long long value) const;
            Variant GetValue(const std::string &key) const;

            static Variant Parse(Type const& type, std::string const& key);
            static std::string GetKey(Type const& type, unsigned long long value);
            static std::vector<std::string> GetKeys(Type const& type);
            static Variant GetValue(Type const& type, std::string const& key);
            static std::vector<Variant> GetValues(Type const& type);

        private:
            Enum(Enum const&) = delete;
            Enum& operator=(Enum const&) = delete;

            friend struct TypeData;

            Enum(const EnumBase *base);

            const EnumBase* m_base;
        };
    }
}