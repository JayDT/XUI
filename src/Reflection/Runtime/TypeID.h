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

#include <xhash>

namespace System::Reflection
{
    inline namespace meta
    {
        struct TypeID
        {
            TypeID() : ID(0), ModulID(0)
            {

            }

            TypeID(uint32_t _id, uint64_t modulid) : ID(_id), ModulID(modulid)
            {
            
            }

            uint32_t ID;
            uint64_t ModulID;

            inline operator uint32_t() const
            {
                return ID;
            }

            inline bool operator==(TypeID const& other) const
            {
                return other.ID == ID && other.ModulID == ModulID ? true : false;
            }
            
            inline bool operator!=(TypeID const& other) const
            {
                return (other.ID != ID || other.ModulID != ModulID) ? true : false;
            }

        };

        const TypeID InvalidTypeID = { 0, 0 };
    }
}

namespace std
{
    template<>
    struct hash<System::Reflection::meta::TypeID>
    {
        size_t operator()(const System::Reflection::meta::TypeID &type) const
        {
            uint64_t const h1(std::hash<uint64_t>()(type.ID));
            uint64_t const h2(std::hash<uint64_t>()(type.ModulID + 1));
            return h1 ^ (h2 << 1);
        }
    };
}

