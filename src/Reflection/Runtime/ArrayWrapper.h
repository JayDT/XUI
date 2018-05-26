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

#include "ArrayWrapperBase.h"

namespace System::Reflection
{
    inline namespace meta
    {
        class Argument;

        class TC_METARUNTIME_API ArrayWrapper
        {
        public:
            ArrayWrapper(void);
            ArrayWrapper(ArrayWrapper const&rhs);
            ~ArrayWrapper();

            template<typename T>
            ArrayWrapper(Array<T>* _array);

            ArrayWrapper& operator=(ArrayWrapper const&rhs);

            void* GetPtr() const;
            Type const* GetType() const;
            Variant GetValue(size_t index) const;
            void SetValue(size_t index, const Argument &value);

            void Insert(size_t index, const Argument &value);
            void Remove(size_t index);

            size_t Size(void) const;

            bool IsValid(void) const;
            bool IsConst(void) const;

            void ReleaseArrayHandle() { m_base = nullptr; }
            ArrayWrapperBase* GetArrayHandle() const { return m_base; }
        private:
            bool m_isConst;

            ArrayWrapperBase *m_base;
        };
    }
}

#include "Impl/ArrayWrapper.hpp"