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

#include "LanguageTypes/Class.h"
#include "LanguageTypes/Constructor.h"

#include "MetaDataConfig.h"
#include "MetaUtils.h"

#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

Constructor::Constructor(
    const Cursor &cursor, 
    const Namespace &currentNamespace, 
    Class *parent
)
    : LanguageType( cursor, currentNamespace )
    , Invokable( cursor )
    , m_parent( parent )
{
    isConvertingConstructor = cursor.Query(clang_CXXConstructor_isConvertingConstructor);
    isCopyConstructor = cursor.Query(clang_CXXConstructor_isCopyConstructor);
    isDefaultConstructor = cursor.Query(clang_CXXConstructor_isDefaultConstructor);
    isMoveConstructor = cursor.Query(clang_CXXConstructor_isMoveConstructor);
}

void Constructor::MetaInit(const Cursor & cursor, const Namespace & currentNamespace)
{
    LanguageType::MetaInit(cursor, currentNamespace);
    Invokable::InitMeta(cursor, m_parent->IsTemplate(), (LanguageType*)(this));
}

bool Constructor::ShouldCompile(void) const
{
    return isAccessible( );
}

Mustache::Data<std::string> Constructor::CompileTemplate(
    const ReflectionParser *context
) const
{
    Mustache::Data<std::string> data { Mustache::Data<std::string>::Type::Object };

    data["internalID"] = std::to_string(++internalID);
    data[ "parentQualifiedName" ] = m_parent->m_qualifiedName;
    data[ "AccessLevel" ] = std::to_string(int(m_accessModifier));

    auto enableNonDynamic = false; // !m_metaData.GetFlag(native_attribute::DisableNonDynamicCtor);
    
    if (enableNonDynamic)
        data[ "templateParameters" ] = getTemplateParameters( false );

    data[ "dynamicTemplateParameters" ] = getTemplateParameters( true );

    data[ "enableNonDynamic" ] = ::utils::TemplateBool( enableNonDynamic );

    std::string invoke_args;
    std::string signature_args;
    for (int i = 0; i != m_signature.size(); ++i)
    {
        if (i)
        {
            invoke_args += " ,";
            signature_args += " ,";
        }
        invoke_args += (boost::format("arguments[%1%]->GetValue< %2% >()") % i % m_signature[i]).str();
        signature_args += (boost::format("typeid(%1%).hash_code()") % m_signature[i]).str();
    }

    data["invoke_args"] = invoke_args;
    data["signature_args"] = signature_args;

    m_metaData.CompileTemplateData( data, context );

    return data;
}

bool Constructor::isAccessible(void) const
{
    // Shared Ref not supported non public constructor
    if (m_accessModifier != CX_CXXPublic)
        return false;

    if (!m_signature.empty() && m_parent->m_templateClass)
        return false;

    // if the parent wants white listed method, then we must have 
    // the enable flag
    if (m_parent->GetMetaData( ).GetFlag( native_attribute::WhiteListMethods ))
        return m_metaData.GetFlag( native_attribute::Enable );

    // must not be explicitly disabled
    return !m_metaData.GetFlag( native_attribute::Disable );
}

std::string Constructor::getTemplateParameters(bool isDynamic) const
{
    std::vector<std::string> params;

    // ClassType
    params.push_back( m_parent->m_qualifiedName );

    // IsDynamic
    params.emplace_back( isDynamic ? "true" : "false" );

    // IsWrapped
    params.emplace_back( 
        m_metaData.GetFlag( native_attribute::DynamicCtorWrap ) ? "true" : "false"
    );

    // Args...
    params.insert( params.end( ), m_signature.begin( ), m_signature.end( ) );

    return boost::join( params, ", " );
}