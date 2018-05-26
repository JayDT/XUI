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

#include "TypeUnpacker.hpp"

namespace std
{
    template<>
    struct hash<System::Reflection::meta::InvokableSignature>
    {
        size_t operator()(
            const System::Reflection::meta::InvokableSignature &signature
        ) const
        {
            hash<System::Reflection::meta::TypeID> hasher;

            size_t seed = 0;

            // combine the hash of all type IDs in the signature
            for (auto &type : signature)
                seed ^= hasher( type->GetID( ) ) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

            return seed;
        }
    };
}
