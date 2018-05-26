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

#include "LanguageTypes/Invokable.h"
#include "LanguageTypes/Class.h"

#include "ReflectionParser.h"
#include "Invokable.h"

Invokable::Invokable(const Cursor &cursor)
    : m_functionType(cursor.GetType())
    , m_returnTypeInfo(cursor.GetReturnType())
    , m_functionAstType(AstType::Create(cursor.GetType()))
    , m_returnAstTypeInfo(AstType::Create(cursor.GetReturnType()))
	, m_guid(cursor.GetUID())
{
    isDefault = cursor.IsDefaulted();
    auto ret = cursor.Query(clang_getCursorAvailability);
    isDeleted = (ret > CXAvailability_Deprecated);

    m_returnType = utils::GetQualifiedName(m_returnTypeInfo.GetCanonicalType()); // .GetDisplayName()
}

Status Invokable::InitMeta(const Cursor & cursor, bool isTemplate, LanguageType * metaData)
{
    CursorType declType = m_returnTypeInfo;
    while (true)
    {
        switch (declType.GetKind())
        {
            case CXType_Pointer:
            case CXType_LValueReference:
            case CXType_RValueReference:
            {
                declType = declType.GetElaboratedType();
                continue;
            }
        }

        break;
    }

    size_t tpos = m_returnType.find("type-parameter-");
    if (tpos != std::string::npos)
    {
        size_t pos = m_returnType.find('<');
        if (pos != std::string::npos)
        {
            std::string returnType = declType.GetDisplayName();
            size_t pos2 = returnType.find('<');
            if (pos2 != std::string::npos)
            {
                size_t startBlock_base = pos2;
                size_t startBlock_canonical = pos;

                size_t finalBlock_canonical = m_returnType.find('>', startBlock_canonical);

                while (startBlock_canonical < finalBlock_canonical)
                {
                    size_t endBlock_base = returnType.find(',', startBlock_base + 1);
                    if (endBlock_base == std::string::npos) endBlock_base = returnType.find('>', startBlock_base + 1);
                    size_t endBlock_canonical = m_returnType.find(',', startBlock_canonical + 1);
                    if (endBlock_canonical == std::string::npos) endBlock_canonical = m_returnType.find('>', startBlock_canonical + 1);

                    returnType.erase(startBlock_base, endBlock_base - startBlock_base);
                    returnType.insert(startBlock_base, m_returnType.substr(startBlock_canonical, endBlock_canonical - startBlock_canonical));

                    endBlock_base = returnType.find(',', startBlock_base);
                    if (endBlock_base == std::string::npos) break; // no more template from base type (auto declare)

                    startBlock_base = endBlock_base;
                    startBlock_canonical = endBlock_canonical;
                }

                m_returnType = returnType;
            }
        }
    }
    else
    {
        if (m_returnTypeInfo.GetDeclaration().GetKind() == CXCursorKind::CXCursor_NoDeclFound)
        {
            if (Class* klass = ReflectionModulParser::Instance->GetClassDesc(m_returnAstTypeInfo->GetDeclarationHash()))
                klass->DeclareTemplate(*m_returnAstTypeInfo, &cursor);
        }
        else if (Class* klass = ReflectionModulParser::Instance->GetClassDesc(m_returnTypeInfo.GetDeclaration()))
            klass->DeclareTemplate(*m_returnAstTypeInfo, &cursor);
    }

    auto type = cursor.GetType();
    unsigned count = type.GetArgumentCount();
    m_callingConvention = type.GetFunctionTypeCallingConv();

    m_signature.clear();

    for (unsigned i = 0; i < count; ++i)
    {
        auto argument = type.GetArgument(i);

        AstType* astArg = AstType::Create(argument);
        m_arguments.push_back(astArg);

        std::string qualifiedInThisParent = utils::GetQualifiedName(argument.GetCanonicalType());
        m_signature.emplace_back(qualifiedInThisParent);
        if (m_signature.back().empty())
            throw Status::Warn("Error: Unknown type");
    }

    return Status();
}
