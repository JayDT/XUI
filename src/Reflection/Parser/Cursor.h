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

#include "CursorType.h"
#include "clang-c/Index.h"

class SourceFileDescriptor;

class Cursor
{
    friend class CursorType;
    friend class SourceFileDescriptor;
public:
    typedef std::vector<Cursor> List;
    typedef CXCursorVisitor Visitor;

    Cursor() {}
    Cursor(const CXCursor &handle);

    CXCursorKind GetKind(void) const;
    CXCursorKind GetTemplateCursorKind(void) const;

    Cursor GetLexicalParent(void) const;
	Cursor GetSemanticParent(void) const;
    Cursor GetTemplateSpecialization(void) const;
    Cursor GetDefinition(void) const;
    Cursor GetCanonical(void) const;

    std::string GetSpelling(void) const;
    std::string GetDisplayName(void) const;
    std::string GetMangledName(void) const;
    std::string GetUSR(void) const;
    std::string GetReferenceName(void) const;
    std::string GetIncludedFile(void) const;

    std::string GetSourceFile(void) const;
    std::string GetNormalizedSourceFile(void) const;
    std::string GetSourceFileInfo(unsigned& line, unsigned& column, unsigned& offset) const;

	std::vector<Cursor> GetOverrides() const;

    bool IsLocalSource(const char* file) const;
    bool IsDefinition(void) const;
    bool IsConst(void) const;
    bool IsStatic(void) const;
    bool isPureVirtual(void) const;
    bool IsVirtualBase(void) const;
    bool IsDynamicCall(void) const;
    bool IsVirtual(void) const;
    bool IsDefaulted(void) const;

    template<typename TFunc, typename TRes = unsigned>
    TRes Query(TFunc func) const
    {
        return TRes(func(m_handle));
    }

    CX_CXXAccessSpecifier GetAccessModifier(void) const;
    CX_StorageClass GetStorageClass(void) const;

    CursorType GetType(void) const;
    CursorType GetReturnType(void) const;
    CursorType GetTypedefType(void) const;
    CursorType GetReceivedType(void) const;

    List GetChildren(void) const;
    void VisitChildren(Visitor visitor, void *data = nullptr);

    size_t GetHash(void) const;
    size_t GetUID(void) const;
    SourceFileDescriptor GetFileDesc(void) const;

    int GetTemplateArgCnt() const;

    bool operator==(const Cursor &obj) const
    {
        return GetUID() == obj.GetUID();
    }

    bool operator!=(const Cursor &obj) const
    {
        return GetUID() != obj.GetUID();
    }

private:
    CXCursor m_handle;
    std::size_t m_uid = 0;
};

struct SourceFileDescriptor
{
    SourceFileDescriptor() {}
    SourceFileDescriptor(Cursor const&);
    SourceFileDescriptor(CXTranslationUnit tu, const char *file_name);

    bool InSystemHeader() const;
    bool InCompiledSource() const;

    std::size_t GetHash(void) const;

    bool IsEqual(SourceFileDescriptor const&) const;

    bool operator==(const SourceFileDescriptor &obj) const
    {
        return GetHash() == obj.GetHash();
    }

    bool operator!=(const SourceFileDescriptor &obj) const
    {
        return GetHash() != obj.GetHash();
    }

    CXFileUniqueID m_uid;
    CXFile file;
    unsigned line = 0, column = 0, offset = 0;
    CXSourceLocation m_source_location;
    std::size_t m_hash = 0;
};

namespace std
{
    template <>
    struct hash<SourceFileDescriptor> 
    {
        std::size_t operator() (SourceFileDescriptor const& uid) const 
        {
            return uid.GetHash();
        }
    };

    template<>
    struct hash<Cursor>
    {
        size_t operator()(const Cursor &obj) const
        {
            return obj.GetUID();
        }
    };
}