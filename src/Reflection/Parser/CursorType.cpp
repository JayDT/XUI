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

#include "clang-c/Index.h"
#include "MetaUtils.h"

#include "Cursor.h"
#include "CursorType.h"
#include "ReflectionParser.h"
#include <mutex>

std::map<size_t, std::shared_ptr<AstType>> AstType::sGlobalTypes;

CursorType::CursorType(const CXType &handle)
    : m_handle( handle )
{

}

std::string CursorType::GetDisplayName(void) const
{
    std::string displayName;

    utils::ToString( clang_getTypeSpelling( m_handle ), displayName );

    return displayName;
}

int CursorType::GetArgumentCount(void) const
{
    return clang_getNumArgTypes( m_handle );
}

CursorType CursorType::GetArgument(unsigned index) const
{
    return clang_getArgType( m_handle, index );
}

CursorType CursorType::GetCanonicalType(void) const
{
    return clang_getCanonicalType( m_handle );
}

CursorType CursorType::GetElaboratedType(void) const
{
    switch (GetKind())
    {
        case CXType_Pointer:
        case CXType_LValueReference:
        case CXType_RValueReference:
            return clang_getPointeeType(m_handle);
    }

    return m_handle;
}

CursorType CursorType::GetNamedType(void) const
{
    return clang_Type_getNamedType(m_handle);
}

CursorType CursorType::GetClassType(void) const
{
    return clang_Type_getClassType(m_handle);
}

Cursor CursorType::GetDeclaration(void) const
{
    return clang_getTypeDeclaration( m_handle );
}

CXRefQualifierKind CursorType::GetAccessSpecifier() const
{
    return clang_Type_getCXXRefQualifier(m_handle);
}

CXTypeKind CursorType::GetKind(void) const
{
    return m_handle.kind;
}

int CursorType::GetTemplateArgumentCount() const
{
    return clang_Type_getNumTemplateArguments ( m_handle );
}

// clang_Cursor_getTemplateArgumentValue
std::vector<CursorType> CursorType::GetTemplateArguments() const
{
    int targCnt = GetTemplateArgumentCount();
    if (targCnt < 1)
        return std::vector<CursorType>();

    std::vector<CursorType> ret;
    for (int i = 0; i != targCnt; ++i)
    {
        CursorType type = (clang_Type_getTemplateArgumentAsType(m_handle, i));
        // ToDo: non type template support
        //if (type.GetKind() == CXTypeKind::CXType_Invalid)
        //{
        //
        //    Cursor decl = GetDeclaration();
        //    if (decl.GetKind() != CXCursorKind::CXCursor_NoDeclFound)
        //    {
        //        Cursor def = decl.GetDefinition();
        //        auto kind = clang_Cursor_getTemplateArgumentKind(def.m_handle, i);
        //        if (kind != CXTemplateArgumentKind::CXTemplateArgumentKind_Invalid)
        //        {
        //            type = (clang_Cursor_getTemplateArgumentType(def.m_handle, i));
        //            if (type.GetKind() == CXTypeKind::CXType_Invalid)
        //            {
        //                auto value = clang_Cursor_getTemplateArgumentValue(def.m_handle, i);
        //                printf("");
        //            }
        //        }
        //    }
        //}

        ret.push_back(std::move(type));
    }

    //clang_Cursor_getTemplateArgumentType

    return ret;
}

CXCallingConv CursorType::GetFunctionTypeCallingConv() const
{
    return clang_getFunctionTypeCallingConv( m_handle );
}

bool CursorType::IsConst(void) const
{
    return clang_isConstQualifiedType( m_handle ) ? true : false;
}

bool CursorType::IsPointer(void) const
{
    return GetKind() == CXType_Pointer;
}

bool CursorType::IsReference(void) const
{
    return GetKind() == CXType_LValueReference;
}

intptr_t CursorType::GetId() const
{
    std::size_t seed = 0;
    std::hash<intptr_t> hasher;
    System::hash_combine(seed, hasher(intptr_t(m_handle.data[0])));
    System::hash_combine(seed, hasher(intptr_t(m_handle.data[1])));
    return seed;
}



///**
//* \brief Returns the number of template arguments for given class template
//* specialization, or -1 if type \c T is not a class template specialization.
//*
//* Variadic argument packs count as only one argument, and can not be inspected
//* further.
//*/
//CINDEX_LINKAGE int clang_Type_getNumTemplateArguments(CXType T);
//
///**
//* \brief Returns the type template argument of a template class specialization
//* at given index.
//*
//* This function only returns template type arguments and does not handle
//* template template arguments or variadic packs.
//*/
//CINDEX_LINKAGE CXType clang_Type_getTemplateArgumentAsType(CXType T, unsigned i);
//
///**
//* \brief Retrieve the ref-qualifier kind of a function or method.
//*
//* The ref-qualifier is returned for C++ functions or methods. For other types
//* or non-C++ declarations, CXRefQualifier_None is returned.
//*/
//CINDEX_LINKAGE enum CXRefQualifierKind clang_Type_getCXXRefQualifier(CXType T);
//clang_getNumArgTypes

void AstType::Initialize(const CursorType & handle)
{
    isConst = handle.IsConst();
    isPointer = handle.IsPointer();
    isReference = handle.IsReference();

    m_typeKind = handle.GetKind();
    m_accessSpecifier = handle.GetAccessSpecifier();

    if (handle.GetKind() == CXType_Typedef || handle.GetKind() == CXType_Unexposed || handle.GetKind() == CXType_Elaborated)
    {
        m_displayName = utils::GetUnqualifiedName(handle);
        m_qualifiedName = utils::GetQualifiedName(handle);
    }
    else if (handle.GetKind() != CXTypeKind::CXType_Invalid)
    {
        Cursor decl = handle.GetDeclaration();
        if (decl.GetKind() != CXCursorKind::CXCursor_NoDeclFound)
        {
            m_displayName = utils::GetUnqualifiedName(decl.GetType()); // GetDisplayName();
            m_qualifiedName = utils::GetQualifiedName(decl.GetType());
        }
        else
        {
            m_displayName = utils::GetUnqualifiedName(handle);
            m_qualifiedName = utils::GetQualifiedName(handle);
        }
    }

    Cursor tType = handle.GetDeclaration();
    if (tType.GetKind() != CXTypeKind::CXType_Unexposed)
        m_declaration = AstType::Create(tType.GetType());

    if (handle.GetCanonicalType().GetKind() != CXCursorKind::CXCursor_FirstInvalid)
        m_canonicalType = AstType::Create(handle.GetCanonicalType());
    if (handle.GetElaboratedType().GetKind() != CXCursorKind::CXCursor_FirstInvalid)
        m_elaborateType = AstType::Create(handle.GetElaboratedType());
    if (handle.GetNamedType().GetKind() != CXCursorKind::CXCursor_FirstInvalid)
        m_namedType = AstType::Create(handle.GetNamedType());
    if (handle.GetClassType().GetKind() != CXCursorKind::CXCursor_FirstInvalid)
        m_classType = AstType::Create(handle.GetClassType());

    //if (cursor.GetKind() != CXCursor_NoDeclFound)
    int iarg = handle.GetArgumentCount();
    if (iarg > 0)
    {
        for (int i = 0; i != iarg; ++i)
            m_arguments.push_back(AstType::Create(handle.GetArgument(i)));
    }

    for (auto _ty : handle.GetTemplateArguments())
        m_templateArgs.push_back(AstType::Create(_ty));

    InitializeDeclaration(handle);
}

void AstType::InitializeDeclaration(const CursorType & handle)
{
    if (!m_declarationHash || !ClassLangType)
    {
        Cursor decl = handle.GetDeclaration();
        if (decl.GetKind() == CXCursorKind::CXCursor_NoDeclFound)
            decl = handle.GetElaboratedType().GetDeclaration();

        if (decl.GetKind() != CXCursorKind::CXCursor_NoDeclFound)
        {
            Cursor def = decl.GetDefinition();
            if (def.GetKind() != CXCursorKind::CXCursor_InvalidFile)
            {
                m_declarationHash = def.GetUID();
                file = def.GetSourceFileInfo(line, column, offset);
            }
            else
            {
                // Example: Template defination not have definition but declaration refered for template type
                m_declarationHash = decl.GetUID();
                file = decl.GetSourceFileInfo(line, column, offset);
            }
        }

        if (m_declarationHash)
        {
            auto iClass = ReflectionModulParser::Instance->m_classes.find(m_declarationHash);
            if (iClass != ReflectionModulParser::Instance->m_classes.end())
                ClassLangType = iClass->second;
        }
    }
}

AstType * AstType::Create(const CursorType & handle)
{
    static std::recursive_mutex sMutex;
    std::lock_guard<std::recursive_mutex> g(sMutex);
    std::hash<std::string> typehash;
    size_t hashid = typehash(handle.GetDisplayName()); //GetDeclaration().GetUID();
    auto type = sGlobalTypes.find(hashid);
    if (type != sGlobalTypes.end())
    {
        if (type->second.get() && !type->second.get()->GetDeclarationHash())
        {
            type->second.get()->InitializeDeclaration(handle);
        }
        return type->second.get();
    }
    auto rType = std::make_shared<AstType>();
    auto& astType = sGlobalTypes[hashid];
    if (astType)
        throw std::exception("invalid type");
    astType = rType;
    astType->Initialize(handle);
    return rType.get();
}

AstType::AstType()
{
}
