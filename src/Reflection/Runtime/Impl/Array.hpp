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
    template<typename T>
    Array<T>::Array(void) { }

    template<typename T>
    Array<T>::Array(const std::vector<T> &rhs)
        : m_storage( rhs ) { }

    template<typename T>
    Array<T>::Array(const std::vector<T> &&rhs)
        : m_storage( std::move( rhs ) ) { }

    template<typename T>
    Array<T>::Array(const std::initializer_list<T> &rhs)
        : m_storage( rhs ) { }

    template<typename T>
    Array<T>::Array(const std::initializer_list<T> &&rhs)
        : m_storage( std::move( rhs ) ) { }

    template<typename T>
    T const * Array<T>::GetPtr() const { return m_storage.data(); }

    template<>
    class Array<bool>
    {
    public:
        Array(void) {}
        Array(const std::vector<bool> &rhs)
            : m_storage(rhs)
        {}
        Array(const std::vector<bool> &&rhs)
            : m_storage(std::move(rhs))
        {}

        Array(const std::initializer_list<bool> &rhs)
            : m_storage(rhs)
        {}

        Array(const std::initializer_list<bool> &&rhs)
            : m_storage(std::move(rhs))
        {}

        bool const* GetPtr() const { return nullptr; }

        std::vector<bool> m_storage;
    };
}