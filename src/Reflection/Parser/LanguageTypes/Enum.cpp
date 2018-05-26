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

#include "LanguageTypes/Enum.h"

Enum::Value::Value(Enum *parent, const Cursor &cursor)
    : key( cursor.GetDisplayName( ) )
    , value( parent->m_qualifiedName + "::" + key )
{

}

Enum::Enum(const Cursor &cursor, const Namespace &currentNamespace)
    : LanguageType( cursor, currentNamespace )
    , m_name( cursor.GetType( ).GetDisplayName( ) )
    , m_qualifiedName( m_name )
{
}

void Enum::MetaInit(const Cursor & cursor, const Namespace & currentNamespace)
{
    LanguageType::MetaInit(cursor, currentNamespace);

    auto displayName = m_metaData.GetNativeString(::native_attribute::DisplayName);

    if (displayName.empty())
        m_displayName = m_qualifiedName;
    else
        m_displayName = utils::GetQualifiedName(cursor, currentNamespace);

    // it's an anonymous enum?
    if (m_displayName.find("anonymous enum") != std::string::npos)
        m_displayName = "";
}

bool Enum::ShouldCompile(void) const
{
    return isAccessible( );
}

Mustache::Data<std::string> Enum::CompileTemplate(const ReflectionParser *context) const
{
    Mustache::Data<std::string> data { Mustache::Data<std::string>::Type::Object };

    data[ "displayName" ] = m_displayName;
    data[ "qualifiedName" ] = m_qualifiedName;
    data[ "ptrTypeEnabled" ] = utils::TemplateBool( m_ptrTypeEnabled );

    data[ "constPtrTypeEnabled" ] = 
        utils::TemplateBool( m_constPtrTypeEnabled );

    Mustache::Data<std::string> members { Mustache::Data<std::string>::Type::List };

    int i = 0;

    for (auto &value : m_values)
    {
        Mustache::Data<std::string> member { Mustache::Data<std::string>::Type::Object };

        member[ "key" ] = value.key;
        member[ "value" ] = value.value;
        member[ "isLast" ] = utils::TemplateBool( i == m_values.size( ) - 1 );

        members << member;

        ++i;
    }

    data[ "member" ] = members;

    m_metaData.CompileTemplateData( data, context );

    return data;
}

bool Enum::isAccessible(void) const
{
    return !m_metaData.GetFlag( native_attribute::Disable );
}