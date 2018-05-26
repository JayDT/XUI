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
        namespace variant_policy
        {
            /** @brief Wraps an Object pointer, rather than copying the value.
             *         This also ensures base class pointers can resolve their inherited type
             */
            struct WrapObject { };

            /** @brief Uses a reference to a value rather than copying it
             */
            struct NoCopy { };
            struct Copy { };
            struct Clone { };
        }
    }
}

#define ReferenceVariantPolicy static_cast<System::Reflection::meta::variant_policy::NoCopy*>(nullptr)
#define CopyVariantPolicy static_cast<System::Reflection::meta::variant_policy::Copy*>(nullptr)