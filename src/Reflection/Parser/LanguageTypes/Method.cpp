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
#include "LanguageTypes/Method.h"

#include "MetaDataConfig.h"
#include "MetaUtils.h"

#include <boost/format.hpp>
#include <boost/algorithm/string/join.hpp>

extern unsigned long internalID;

Method::Method(
    const Cursor &cursor, 
    const Namespace &currentNamespace, 
    Class *parent
)
    : MethodBase( cursor, currentNamespace, parent )
{
    m_isConst = (cursor.IsConst());
}

Method::~Method(void)
{
}

void Method::MetaInit(const Cursor & cursor, const Namespace & currentNamespace)
{
    MethodBase::MetaInit(cursor, currentNamespace);

	// m_overrides

	auto overrides = cursor.GetOverrides();
	if (!overrides.empty())
	{
		for (auto o : overrides)
		{
			Class* _classPtr = nullptr;
			auto& m_ovd = m_overrided.emplace_back();
			auto _class = o.GetSemanticParent();
			m_ovd.m_storageClassAstType = AstType::Create(_class.GetType());
			m_ovd.m_baseClass = m_ovd.m_storageClassAstType->FindClassLangType();
			if (!m_ovd.m_baseClass)
			{
				m_ovd.m_storageClassAstType->InitializeDeclaration(_class.GetType());
				m_ovd.m_baseClass = m_ovd.m_storageClassAstType->FindClassLangType();
				if (!m_ovd.m_baseClass)
				{
					//auto type_name = _class.GetDisplayName();
					//	//::utils::GetQualifiedName(_class.GetType());
					//_classPtr = ReflectionModulParser::Instance->GetClassDescByName(type_name.c_str());
				}
				else
					_classPtr = m_ovd.m_baseClass.get();
				//EXCEPT(m_ovd.m_baseClass && "Invalid type");
			}
			else
				_classPtr = m_ovd.m_baseClass.get();

			if (_classPtr)
			{
				m_ovd.m_baseMethod = _classPtr->GetMethod(o.GetUID());
				m_parent->ResolveOverrideMethod(m_ovd.m_baseMethod, this);
			}
		}
	}
}

bool Method::ShouldCompile(void) const
{
    return isAccessible( ) && !m_isFunctionTemplate;
}

void Method::DeclareTemplate(CursorType const & defination)
{
}

void Method::BuildTemplateDefinition()
{
}

Mustache::Data<std::string> Method::CompileTemplate(const ReflectionParser *context) const
{
    Mustache::Data<std::string> data { Mustache::Data<std::string>::Type::Object };

    MethodBase::CompileStandardTemplate(data, context);

    return data;
}

bool Method::isAccessible(void) const
{
    if (/*m_accessModifier != CX_CXXPublic ||*/ m_isPureVirtual)
        return false;

    // if the parent wants white listed method, then we must have 
    // the enable flag
    if (m_parent->GetMetaData( ).GetFlag( native_attribute::WhiteListMethods ))
        return m_metaData.GetFlag( native_attribute::Enable );

    // must not be explicitly disabled
    return !m_metaData.GetFlag( native_attribute::Disable );
}

std::string Method::getQualifiedSignature(std::string const& type_name) const
{
    auto argsList = boost::join(m_signature, ", ");

    std::string constNess = m_isConst ? " const" : "";

    return (boost::format("%1%(%2%::*%5%)(%3%)%4%") %
        m_returnType %
        m_parent->m_qualifiedName %
        argsList % constNess %
        type_name
        ).str();
}