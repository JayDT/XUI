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
#include "ArrayWrapperBase.h"
#include "Argument.h"
#include "Array.h"

namespace System::Reflection
{
    inline namespace meta
    {
        class Argument;

        template<typename T>
        class ArrayWrapperContainer : public ArrayWrapperBase
        {
        public:
            ArrayWrapperContainer(Array<T> &a);

            void* GetPtr() override;
            Type const* GetType() override;
            Variant GetValue(size_t index) override;
            void SetValue(size_t index, const Argument &value) override;

            void Insert(size_t index, const Argument &value) override;
            void Remove(size_t index) override;

            void Resize(size_t newSize) override { m_array.m_storage.resize(newSize); }
            void Reserve(size_t newSize) override { m_array.m_storage.reserve(newSize); }

            size_t Size(void) const override;
            
        private:
            Array<T> &m_array;
        };
    }
}

#include "Impl/ArrayWrapperContainer.hpp"