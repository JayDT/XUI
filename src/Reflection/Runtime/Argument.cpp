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

#include "Argument.h"
#include "Type.h"
#include "Variant.h"

#include "ReflectionDatabase.h"
#include "ReflectionModule.h"

namespace System::Reflection
{
    inline namespace meta
    {
        Argument::Argument(void)
            : m_data()
        { }

        Argument::Argument(const Argument &rhs) 
            : m_data( rhs.m_data ) { }

        Argument::Argument(Variant &obj) 
            : m_data( obj )
        {
            // TODO
            //obj.ToSharedObject() != nullptr;
        }

        Argument::Argument(const Variant &obj) 
            : m_data( obj ) { }

        Argument &Argument::operator=(const Argument &rhs) 
        {
            m_data = rhs.m_data;
            return *this;
        }

        Type const* Argument::GetType(void) const
        {
            return m_data.GetType();
        }

        Object * Argument::ToObject(void) const
        {
            if (m_data.GetBase() && m_data.IsObject())
                return m_data.ToObject();
            return nullptr;
        }

        RefObject Argument::ToSharedObject(void) const
        {
            if (m_data.GetBase() && m_data.GetBase()->IsSharedPointer() && m_data.IsObject())
                return m_data.ToSharedObject();
            return System::Reflection::RefObject(nullptr);
        }

        std::shared_ptr<void> Argument::ToSharedPointer(void) const
        {
            if (m_data.GetBase() && m_data.GetBase()->IsSharedPointer() && m_data.IsObject())
                return m_data.ToSharedPointer();
            return std::shared_ptr<void>(nullptr);
        }

        void *Argument::GetPtr(void) const
        {
            return m_data.getPtr();
        }
    }
}
