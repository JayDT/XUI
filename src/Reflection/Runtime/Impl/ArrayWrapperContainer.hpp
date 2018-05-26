#include "..\ArrayWrapperContainer.h"
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
    inline namespace meta
    {
        template<typename T>
        ArrayWrapperContainer<T>::ArrayWrapperContainer(Array<T> &a)
            : m_array( a )
        {

        }

        template<typename T>
        void * ArrayWrapperContainer<T>::GetPtr() { return (void*)(m_array.GetPtr()); }

        template<typename T>
        Type const * ArrayWrapperContainer<T>::GetType()
        {
            return &System::Reflection::type_of(typeid(T));
        }

        template<typename T>
        Variant ArrayWrapperContainer<T>::GetValue(size_t index)
        {
            return Variant(m_array.m_storage[ index ]);
        }

        template<typename T>
        void ArrayWrapperContainer<T>::SetValue(size_t index, const Argument &value)
        {
            m_array.m_storage.at( index ) = *(T*)(value.GetPtr( ));
        }

        template<typename T>
        void ArrayWrapperContainer<T>::Insert(size_t index, const Argument &value)
        {
            m_array.m_storage.insert( m_array.m_storage.begin( ) + index, *(T*)(value.GetPtr()));
        }

        template<typename T>
        void ArrayWrapperContainer<T>::Remove(size_t index)
        {
            m_array.m_storage.erase( m_array.m_storage.begin( ) + index );
        }

        template<typename T>
        size_t ArrayWrapperContainer<T>::Size(void) const
        {
            return m_array.m_storage.size( );
        }
    }
}