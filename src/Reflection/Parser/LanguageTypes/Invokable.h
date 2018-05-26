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

#pragma once

#include "Cursor.h"
#include "Namespace.h"
#include "MetaDataManager.h"
#include "Logger.h"

class LanguageType;

class Invokable
{
public:
    typedef std::vector<std::string> Signature;

    Invokable(const Cursor &cursor);

    CXCallingConv GetCallingConv() const { return m_callingConvention; }
    CursorType const& GetReturnType() const { return m_returnTypeInfo; }
    AstType* GetFunctionAstType() const { return m_functionAstType; }
    AstType* GetReturnAstType() const { return m_returnAstTypeInfo; }
    std::vector<AstType*> const& GetArgumentTypes() const { return m_arguments; }

    Status InitMeta(const Cursor &cursor, bool isTemplate, LanguageType* metaData);

    bool isDefault = false;
    bool isDeleted = false;
	size_t m_guid = 0;

protected:
    CursorType m_functionType;
    CursorType m_returnTypeInfo;
    CXCallingConv m_callingConvention;

    AstType* m_functionAstType;
    AstType* m_returnAstTypeInfo;

    std::string m_returnType;

    std::vector<AstType*> m_arguments;
    Signature m_signature;
};

const auto kReturnTypeVoid = "void";