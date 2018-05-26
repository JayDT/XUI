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

#include "EnumBase.h"

#include "Variant.h"
#include "Argument.h"

#include "ReflectionModule.h"

namespace System::Reflection
{
    inline namespace meta
    {
        EnumBase::EnumBase(const std::string &name, TypeID owner)
            : m_parentType( ReflectionModulManager::Instance().GetType(owner) )
            , m_name( name ) { }

        Type const* EnumBase::GetParentType(void) const
        {
            return &m_parentType;
        }

        const std::string &EnumBase::GetName(void) const
        {
            return m_name;
        }
    }
}