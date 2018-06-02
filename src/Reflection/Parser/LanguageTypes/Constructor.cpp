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

    // TODO: Build template
    std::string invoke_args;
    std::string signature_args;
    for (int i = 0; i != m_signature.size(); ++i)
    {
        if (i)
        {
            invoke_args += " ,";
            signature_args += " ,";
        }

        bool hasVirtualBase = false;
        bool isReflObject = false;
        bool isPointer = m_arguments[i]->IsPointer();
        auto ArgTypeClass = m_arguments[i]->FindClassLangType();  //GetDeclarationType()->GetKind() != CXTypeKind::CXType_Invalid ? m_arguments[i]->GetDeclarationType()->GetClassLangType() : m_arguments[i]->GetClassLangType();
        if (ArgTypeClass && ArgTypeClass->IsTemplate() && ArgTypeClass->GetDisplayName() == "shared_ptr")
        {
            data["isTypeReflPointer"] = utils::TemplateBool(true);
            for (auto defClass : ArgTypeClass->m_definations)
            {
                //if (m_arguments[i]->GetQualifiedName() == defClass->m_qualifiedName)
                if (defClass->m_astType && (m_arguments[i] == defClass->m_astType || m_arguments[i]->GetElaboratedType() == defClass->m_astType || m_arguments[i]->GetElaboratedType()->GetDeclarationType() == defClass->m_astType))
                {
                    // Special case for std::shared_ptr
                    if (!defClass->m_templateTypes.empty())
                    {
                        auto templateArgClass = defClass->m_templateTypes[0]->GetClassLangType().get();

                        if (!templateArgClass)
                        {
                            auto hash = defClass->m_templateTypes[0]->GetDeclarationHash();
                            if (defClass->m_templateTypes[0]->GetElaboratedType())
                                hash = defClass->m_templateTypes[0]->GetElaboratedType()->GetDeclarationHash();

                            templateArgClass = ReflectionModulParser::Instance->GetClassDesc(hash);
                        }

                        if (templateArgClass)
                        {
                            // Note: This class not part of assembly but required init for correct typeinfo
                            if (!templateArgClass->IsPreparedForAssembly())
                                templateArgClass->PrepareForAssembly();
                            isReflObject = templateArgClass->IsReflObject();
                            hasVirtualBase = templateArgClass->HasVirtualBase();
                            break;
                        }
                    }
                }
            }

            if (!isPointer && isReflObject)
            {
                if (hasVirtualBase)
                    invoke_args += "std::dynamic_pointer_cast";
                else
                    invoke_args += "std::static_pointer_cast";

                invoke_args += (boost::format("<System::Reflection::meta::CleanedType<%2%>>(arguments[%1%]->ToSharedObject())") % i % m_signature[i]).str();
            }
            else
            {
                invoke_args += (boost::format("std::static_pointer_cast<System::Reflection::meta::CleanedType<%2%>>(arguments[%1%]->ToSharedPointer())") % i % m_signature[i]).str();
            }
        }
        else
        {
            if (m_arguments[i]->GetElaboratedType())
            {
                Class* klass = ReflectionModulParser::Instance->GetClassDesc(m_arguments[i]->GetElaboratedType()->GetDeclarationHash());
                if (klass)
                {
                    // Note: This class not part of assembly but required init for correct typeinfo
                    if (!klass->IsPreparedForAssembly())
                        klass->PrepareForAssembly();
                    isReflObject = klass->IsReflObject();
                    hasVirtualBase = klass->HasVirtualBase();
                }
            }

            if (isReflObject)
            {
                if (hasVirtualBase)
                {
                    if (isPointer)
                        invoke_args += (boost::format("dynamic_cast<std::remove_reference<%2%>::type>(arguments[%1%]->ToObject())") % i % m_signature[i]).str();
                    else
                        invoke_args += (boost::format("*dynamic_cast<std::remove_reference<%2%>::type*>(arguments[%1%]->ToObject())") % i % m_signature[i]).str();
                }
                else
                {
                    if (isPointer)
                        invoke_args += (boost::format("static_cast<std::remove_reference<%2%>::type>(arguments[%1%]->ToObject())") % i % m_signature[i]).str();
                    else
                        invoke_args += (boost::format("*static_cast<std::remove_reference<%2%>::type*>(arguments[%1%]->ToObject())") % i % m_signature[i]).str();
                }
            }
            else
            {
                invoke_args += (boost::format("*static_cast<std::remove_reference<%2%>::type*>(arguments[%1%]->GetPtr())") % i % m_signature[i]).str();
            }
        }
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