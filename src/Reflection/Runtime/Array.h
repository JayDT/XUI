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

#include <vector>

namespace System::Reflection
{
    // Basic wrapper around std::vector. Clang has a hard time with other 
    // compilers implementation of std::vector, so this must be used in order to have 
    // Array like functionality within the runtime
    template<typename T>
    class Array
    {
    public:
        Array(void);
        Array(const std::vector<T> &rhs);
        Array(const std::vector<T> &&rhs);
        Array(const std::initializer_list<T> &rhs);
        Array(const std::initializer_list<T> &&rhs);

        T const* GetPtr() const;

        std::vector<T> m_storage;
    };
}

#include "Impl/Array.hpp"