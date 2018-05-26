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

#include "Variant.h"
#include "Array.h"

namespace System::Reflection
{
    inline namespace meta
    {
        class Argument;
        class Type;

        class TC_METARUNTIME_API ArrayWrapperBase
        {
        public:
            virtual ~ArrayWrapperBase(void) { }

            virtual void* GetPtr() = 0;
            virtual Type const* GetType() = 0;
            virtual Variant GetValue(size_t index) = 0;
            virtual void SetValue(size_t index, const Argument &value) = 0;
            virtual void Insert(size_t index, const Argument &value) = 0;
            virtual void Remove(size_t index) = 0;
            virtual void Resize(size_t newSize) = 0;
            virtual void Reserve(size_t newSize) = 0;

            virtual size_t Size(void) const = 0;
        };
    }
}