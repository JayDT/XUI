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

#include "Precompiled.h"

#include "ArrayWrapper.h"
#include "Type.h"

#include "Common/Logging.h"

namespace System::Reflection
{
    inline namespace meta
    {
        ArrayWrapper::ArrayWrapper(void)
            : m_isConst( true )
            , m_base( nullptr ) { }

        ArrayWrapper::ArrayWrapper(ArrayWrapper const & rhs)
        {
            m_isConst = rhs.m_isConst;
            m_base = rhs.m_base;
            const_cast<ArrayWrapper&>(rhs).m_base = nullptr;
        }

        ArrayWrapper::~ArrayWrapper()
        {
            if (m_base)
                delete m_base;
        }

        ArrayWrapper& ArrayWrapper::operator=(ArrayWrapper const&rhs)
        {
            m_isConst = rhs.m_isConst;
            m_base = rhs.m_base;
            const_cast<ArrayWrapper&>(rhs).m_base = nullptr;
            return *this;
        }

        void * ArrayWrapper::GetPtr() const { return m_base ? m_base->GetPtr() : nullptr; }

        Type const * ArrayWrapper::GetType() const { return m_base ? m_base->GetType() : &Type::Invalid(); }

        Variant ArrayWrapper::GetValue(size_t index) const
        {
            return m_base ? m_base->GetValue( index ) : Variant( );
        }

        void ArrayWrapper::SetValue(size_t index, const Argument &value)
        {
            UAssert( !m_isConst, "Array is const." );

            if (m_base)
                m_base->SetValue( index, value );
        }

        void ArrayWrapper::Insert(size_t index, const Argument &value)
        {
            UAssert( !m_isConst, "Array is const." );

            if (m_base)
                m_base->Insert( index, value );
        }

        void ArrayWrapper::Remove(size_t index)
        {
            UAssert( !m_isConst, "Array is const." );

            if (m_base)
                m_base->Remove( index );
        }

        size_t ArrayWrapper::Size(void) const
        {
            return m_base ? m_base->Size( ) : 0;
        }

        bool ArrayWrapper::IsValid(void) const
        {
            return m_base != nullptr;
        }

        bool ArrayWrapper::IsConst(void) const
        {
            return m_isConst;
        }
    }
}