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

#include "MetaDataConfig.h"
#include "MetaUtils.h"

#include "LanguageTypes/Global.h"
#include "LanguageTypes/Class.h"

Global::Global(
    const Cursor &cursor, 
    const Namespace &currentNamespace, 
    Class *parent
)
    : FieldBase( cursor, currentNamespace, parent )
    , m_isConst( cursor.GetType( ).IsConst( ) )
{
}

void Global::MetaInit(const Cursor & cursor, const Namespace & currentNamespace)
{
    FieldBase::MetaInit(cursor, currentNamespace);

    std::string m_qualifiedName(::utils::GetQualifiedName(cursor, currentNamespace));
    auto displayName = m_metaData.GetNativeString(::native_attribute::DisplayName);

    if (displayName.empty())
    {
        m_displayName = m_qualifiedName;
    }
    else
    {
        m_displayName =
            ::utils::GetQualifiedName(displayName, currentNamespace);
    }
}

bool Global::ShouldCompile(void) const
{
    return isAccessible( );
}

Mustache::Data<std::string> Global::CompileTemplate(const ReflectionParser *context) const
{
    Mustache::Data<std::string> data = { Mustache::Data<std::string>::Type::Object };

    FieldBase::CompileStandardTemplate(data, context);

    //data[ "qualifiedName" ] = m_qualifiedName;

    return data;
}

bool Global::isAccessible(void) const
{
    return isGetterAccessible() || isSetterAccessible();
}

bool Global::isGetterAccessible(void) const
{
    if (m_enabled || (m_parent && m_parent->isAccessible()))
    {
        if (m_parent)
            return m_hasExplicitGetter || !m_metaData.GetFlag(::native_attribute::Disable);

        return true;
    }

    return false;
}

bool Global::isSetterAccessible(void) const
{
    if (m_isConst)
        return false;

    if (m_enabled || (m_parent && m_parent->isAccessible()))
    {
        if (m_parent)
            return m_hasExplicitSetter || !m_metaData.GetFlag(::native_attribute::Disable);

        return true;
    }

    return false;
}