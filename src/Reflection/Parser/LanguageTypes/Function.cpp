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

#include "LanguageTypes/Function.h"
#include "LanguageTypes/Class.h"

#include "MetaDataConfig.h"
#include "MetaUtils.h"

#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

Function::Function(
    const Cursor &cursor, 
    const Namespace &currentNamespace, 
    Class *parent
)
    : MethodBase( cursor, currentNamespace, parent )
{
}

void Function::MetaInit(const Cursor & cursor, const Namespace & currentNamespace)
{
    MethodBase::MetaInit(cursor, currentNamespace);

    std::string m_qualifiedName(::utils::GetQualifiedName(cursor, currentNamespace));
    m_displayName = m_qualifiedName;
    if (m_qualifiedName.empty())
    {
        m_displayName = cursor.GetSpelling();
        m_qualifiedName = m_displayName;
    }
}

bool Function::ShouldCompile(void) const
{
    return isAccessible( );
}

void Function::DeclareTemplate(CursorType const & defination)
{
}

void Function::BuildTemplateDefinition()
{
}

Mustache::Data<std::string> Function::CompileTemplate(const ReflectionParser *context) const
{
    Mustache::Data<std::string> data { Mustache::Data<std::string>::Type::Object };

    MethodBase::CompileStandardTemplate(data, context);

    //data[ "name" ] = m_name;
    //data[ "qualifiedName" ] = m_qualifiedName;
    //data[ "AccessLevel" ] = std::to_string(int(m_accessModifier));
    //data[ "LinkerVisibility" ] = utils::TemplateBool(m_visibility);
    //
    //if (m_parent)
    //    data[ "parentQualifiedName" ] = m_parent->m_qualifiedName;
    //
    //data[ "qualifiedSignature" ] = getQualifiedSignature( "" );
    //data["typedQualifiedSignature"] = getQualifiedSignature("Signature");
    //
    //data["internalID"]      = std::to_string(++internalID);
    //data["isTemplate"]      = ::utils::TemplateBool(m_isFunctionTemplate);
    //data["isVoidReturn"]    = ::utils::TemplateBool(m_returnType == "void");
    //data["isReferenceReturnType"] = ::utils::TemplateBool(m_isReferenceReturnType);
    //data["returnType"] = m_returnType;
    //
    //std::string invoke_args;
    //std::string signature_args;
    //for (int i = 0; i != m_signature.size(); ++i)
    //{
    //    if (i)
    //    {
    //        invoke_args += " ,";
    //        signature_args += " ,";
    //    }
    //
    //    bool hasVirtualBase = false;
    //    bool isReflObject = false;
    //
    //    if (m_arguments[i]->GetElaboratedType())
    //    {
    //        Class* klass = ReflectionModulParser::Instance->GetClassDesc(m_arguments[i]->GetElaboratedType()->GetDeclarationHash());
    //        if (klass)
    //        {
    //            isReflObject = klass->IsReflObject();
    //            hasVirtualBase = klass->HasVirtualBase();
    //        }
    //    }
    //
    //    if (isReflObject && hasVirtualBase)
    //        invoke_args += (boost::format("*dynamic_cast<std::remove_reference<%2%>::type*>(arguments[%1%]->GetPtr())") % i % m_signature[i]).str();
    //    else
    //        invoke_args += (boost::format("*static_cast<std::remove_reference<%2%>::type*>(arguments[%1%]->GetPtr())") % i % m_signature[i]).str();
    //    signature_args += (boost::format("typeid(%1%).hash_code()") % m_signature[i]).str();
    //}
    //
    //data["invoke_args"] = invoke_args;
    //data["signature_args"] = signature_args;
    //
    //m_metaData.CompileTemplateData( data, context );

    return data;
}

bool Function::isAccessible(void) const
{
    if (m_isFunctionTemplate)
        return false;

    if (m_parent && m_parent->isAccessible())
        return true;

    return m_enabled;
}

std::string Function::getQualifiedSignature(std::string const& typed_name) const
{
    auto argsList = boost::join( m_signature, ", " );

    return (boost::format( "%1%(*%3%)(%2%)" ) % m_returnType % argsList % typed_name).str( );
}