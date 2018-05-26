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

class Class;
class Cursor;

class CursorType
{
public:
    CursorType() {}
    CursorType(const CXType &handle);

    std::string GetDisplayName(void) const;

    int GetArgumentCount(void) const;
    CursorType GetArgument(unsigned index) const;

    CursorType GetCanonicalType(void) const;
    CursorType GetElaboratedType(void) const;
    CursorType GetNamedType(void) const;
    CursorType GetClassType(void) const;

    Cursor GetDeclaration(void) const;

    CXRefQualifierKind GetAccessSpecifier() const;

    CXTypeKind GetKind(void) const;

    int GetTemplateArgumentCount() const;
    std::vector<CursorType> GetTemplateArguments() const;

    CXCallingConv GetFunctionTypeCallingConv() const;

    bool IsConst(void) const;
    bool IsPointer(void) const;
    bool IsReference(void) const;

    bool operator==(CursorType const& other) const
    {
        return m_handle.data[0] == other.m_handle.data[0] && m_handle.data[1] == other.m_handle.data[1];
    }

    intptr_t GetId() const;
private:
    CXType m_handle;
};

class AstType
{
    bool isConst : 1;
    bool isPointer : 1;
    bool isReference : 1;

    CXTypeKind m_typeKind;
    CXRefQualifierKind m_accessSpecifier;
    std::size_t m_declarationHash = 0;
    std::string m_displayName;
    std::string m_qualifiedName;
    AstType* m_canonicalType = nullptr;
    AstType* m_elaborateType = nullptr;
    AstType* m_namedType = nullptr;
    AstType* m_classType = nullptr;
    AstType* m_declaration = nullptr;
    std::vector<AstType*> m_arguments;
    std::vector<AstType*> m_templateArgs;
    mutable std::shared_ptr<Class> ClassLangType;

    // debug
    std::string file;
    unsigned line, column, offset;

    void Initialize(const CursorType &handle);

public:
    static std::map<size_t, std::shared_ptr<AstType>> sGlobalTypes;
    static AstType* Create(const CursorType &handle);

    AstType();
    ~AstType()
    {

    }

    std::string const& GetDisplayName(void) const { return m_displayName; }
    std::string const& GetQualifiedName(void) const { return m_qualifiedName; }

    int GetArgumentCount(void) const { return m_arguments.size(); }
    AstType* GetArgument(unsigned index) const { return m_arguments[index]; }

    AstType* GetCanonicalType(void) const { return m_canonicalType; }
    AstType* GetElaboratedType(void) const { return m_elaborateType; }
    AstType* GetNamedType(void) const { return m_namedType; }
    AstType* GetClassType(void) const { return m_classType; }
    AstType* GetDeclarationType(void) const { return m_declaration; }
    std::shared_ptr<Class>& GetClassLangType() const { return ClassLangType; }

    size_t GetDeclarationHash(void) const { return m_declarationHash; }

    CXRefQualifierKind GetAccessSpecifier() const { return m_accessSpecifier; }

    CXTypeKind GetKind(void) const { return m_typeKind; }

    int GetTemplateArgumentCount() const { return m_templateArgs.size(); }
    std::vector<AstType*> const& GetTemplateArguments() const { return m_templateArgs; }

    std::shared_ptr<Class>& FindClassLangType() const
    {
        auto& klass = GetDeclarationType()->GetKind() != CXTypeKind::CXType_Invalid ? GetDeclarationType()->GetClassLangType() : GetClassLangType();
        return klass;
    }

    bool IsConst(void) const { return isConst; }
    bool IsPointer(void) const { return isPointer; }
    bool IsReference(void) const { return isReference; }

	void InitializeDeclaration(const CursorType &handle);
};