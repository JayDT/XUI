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
#include "Cursor.h"
#include "MetaUtils.h"

#include <boost/functional/hash.hpp>

//TODO: clang_getInclusions

Cursor::Cursor(const CXCursor &handle)
    : m_handle(handle) { }

CXCursorKind Cursor::GetKind(void) const
{
    return m_handle.kind;
}

CXCursorKind Cursor::GetTemplateCursorKind(void) const
{
    return clang_getTemplateCursorKind(m_handle);
}

Cursor Cursor::GetLexicalParent(void) const
{
    return clang_getCursorLexicalParent(m_handle);
}

Cursor Cursor::GetTemplateSpecialization(void) const
{
    return clang_getSpecializedCursorTemplate(m_handle);
}

Cursor Cursor::GetDefinition(void) const
{
    return clang_getCursorDefinition(m_handle);
}

Cursor Cursor::GetCanonical(void) const
{
    return clang_getCanonicalCursor(m_handle);
}

Cursor Cursor::GetSemanticParent() const
{
	return clang_getCursorSemanticParent(m_handle);
}

std::string Cursor::GetSpelling(void) const
{
    std::string spelling;

    utils::ToString(clang_getCursorSpelling(m_handle), spelling);

    return spelling;
}

std::string Cursor::GetDisplayName(void) const
{
    std::string displayName;

    utils::ToString(clang_getCursorDisplayName(m_handle), displayName);

    return displayName;
}

std::string Cursor::GetMangledName(void) const
{
    std::string mangled;

    utils::ToString(clang_Cursor_getMangling(m_handle), mangled);

    return mangled;
}

std::string Cursor::GetUSR(void) const
{
    std::string usr;

    utils::ToString(clang_getCursorUSR(m_handle), usr);

    return usr;
}

std::string Cursor::GetReferenceName(void) const
{
    CXSourceRange range = clang_getCursorReferenceNameRange(m_handle, CXNameRefFlags::CXNameRange_WantQualifier | CXNameRefFlags::CXNameRange_WantTemplateArgs, 0);
    if (!clang_Range_isNull(range))
    {
        CXIndex index = clang_createIndex(0, 0);
        CXTranslationUnit TU = clang_Cursor_getTranslationUnit(m_handle);
        //CXSourceRange range = clang_getCursorExtent(clang_getTranslationUnitCursor(TU));

        // Tokenize.
        CXToken *tokens;
        unsigned int numTokens;
        clang_tokenize(TU, range, &tokens, &numTokens);

        for (int i = 0; i < numTokens; ++i) {
            CXToken token = tokens[i];
            switch (clang_getTokenKind(token)) {
                case CXToken_Punctuation:
                {
                    CXString cxstr = clang_getTokenSpelling(TU, token);
                    std::string str;
                    utils::ToString(cxstr, str);
                    std::cout << "CXToken_Punctuation  " << str << std::endl;
                    break;
                }
                case CXToken_Keyword:
                {
                    CXString cxstr = clang_getTokenSpelling(TU, token);
                    std::string str;
                    utils::ToString(cxstr, str);
                    std::cout << "CXToken_Keyword  " << str << std::endl;
                    break;
                }
                case CXToken_Identifier:
                {
                    CXString cxstr = clang_getTokenSpelling(TU, token);
                    std::string str;
                    utils::ToString(cxstr, str);
                    std::cout << "CXToken_Identifier  " << str << std::endl;
                    break;
                }
                case CXToken_Literal:
                {
                    CXString cxstr = clang_getTokenSpelling(TU, token);
                    std::string str;
                    utils::ToString(cxstr, str);
                    std::cout << "CXToken_Literal  " << str << std::endl;
                    break;
                }
                case CXToken_Comment:
                {
                    CXString cxstr = clang_getTokenSpelling(TU, token);
                    std::string str;
                    utils::ToString(cxstr, str);
                    std::cout << "CXToken_Comment  " << str << std::endl;
                    break;
                }
                default:
                    std::cout << "Unknown" << std::endl;
            }
        }

        clang_disposeTokens(TU, tokens, numTokens);
        clang_disposeIndex(index);
    }
    return std::string();
}

std::string Cursor::GetIncludedFile(void) const
{
    std::string filename;
    CXFile file = clang_getIncludedFile(m_handle);
    utils::ToString(clang_getFileName(file), filename);
    return filename;
}

std::string Cursor::GetSourceFile(void) const
{
    if (GetKind() == CXCursorKind::CXCursor_NoDeclFound)
        return "";

    auto range = clang_Cursor_getSpellingNameRange(m_handle, 0, 0);

    auto start = clang_getRangeStart(range);

    CXFile file;
    unsigned line, column, offset;

    clang_getFileLocation(start, &file, &line, &column, &offset);

    std::string filename;

    utils::ToString(clang_getFileName(file), filename);

    return filename;
}

std::string Cursor::GetNormalizedSourceFile(void) const
{
    System::String sourceFile = GetSourceFile().c_str();
    sourceFile = sourceFile.Replace('/', '\\').ToLower();
    return sourceFile;
}

std::string Cursor::GetSourceFileInfo(unsigned & line, unsigned & column, unsigned & offset) const
{
    if (GetKind() == CXCursorKind::CXCursor_NoDeclFound)
        return "";

    auto range = clang_Cursor_getSpellingNameRange(m_handle, 0, 0);

    auto start = clang_getRangeStart(range);

    CXFile file;

    clang_getFileLocation(start, &file, &line, &column, &offset);

    std::string filename;

    if (file)
        utils::ToString(clang_getFileName(file), filename);

    return filename;
}

std::vector<Cursor> Cursor::GetOverrides() const
{
	std::vector<Cursor> overrides;
	CXCursor *overridden;
	unsigned num_overridden;
	clang_getOverriddenCursors(m_handle, &overridden, &num_overridden);
	for (int i = 0; i < num_overridden; ++i)
	{
		overrides.push_back(overridden[i]);
	}
	return std::move(overrides);
}

bool Cursor::IsLocalSource(const char * file) const
{
    System::String sourceFile = GetSourceFile().c_str();
    System::String inputFile = file;

    sourceFile = sourceFile.Replace('/', '\\').ToLower();
    inputFile = inputFile.Replace('/', '\\').ToLower();

    return sourceFile == inputFile;
}

bool Cursor::IsDefinition(void) const
{
    return clang_isCursorDefinition(m_handle) ? true : false;
}

bool Cursor::IsConst(void) const
{
    return clang_CXXMethod_isConst(m_handle) ? true : false;
}

bool Cursor::IsStatic(void) const
{
    return clang_CXXMethod_isStatic(m_handle) ? true : false;
}

bool Cursor::isPureVirtual(void) const
{
    return clang_CXXMethod_isPureVirtual(m_handle);
}

bool Cursor::IsVirtualBase(void) const
{
    return clang_isVirtualBase(m_handle);
}

bool Cursor::IsDynamicCall(void) const
{
    return clang_Cursor_isDynamicCall(m_handle);
}

bool Cursor::IsVirtual(void) const
{
    return clang_CXXMethod_isVirtual(m_handle);
}

bool Cursor::IsDefaulted(void) const
{
    return clang_CXXMethod_isDefaulted(m_handle);
}

CX_CXXAccessSpecifier Cursor::GetAccessModifier(void) const
{
    return clang_getCXXAccessSpecifier(m_handle);
}

CX_StorageClass Cursor::GetStorageClass(void) const
{
    return clang_Cursor_getStorageClass(m_handle);
}

CursorType Cursor::GetType(void) const
{
    return clang_getCursorType(m_handle);
}

CursorType Cursor::GetReturnType(void) const
{
    return clang_getCursorResultType(m_handle);
}

CursorType Cursor::GetTypedefType(void) const
{
    return clang_getTypedefDeclUnderlyingType(m_handle);
}

CursorType Cursor::GetReceivedType(void) const
{
    return clang_Cursor_getReceiverType(m_handle);
}

Cursor::List Cursor::GetChildren(void) const
{
    List children;

    auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData data)
    {
        auto container = static_cast<List *>(data);

        container->emplace_back(cursor);

        if (cursor.kind == CXCursorKind::CXCursor_LastPreprocessing)
            return CXChildVisit_Break;

        return CXChildVisit_Continue;
    };

    clang_visitChildren(m_handle, visitor, &children);

    return children;
}

void Cursor::VisitChildren(Visitor visitor, void *data)
{
    clang_visitChildren(m_handle, visitor, data);
}

size_t Cursor::GetHash(void) const
{
    return clang_hashCursor(m_handle);
}

size_t Cursor::GetUID(void) const
{
    if (!m_uid && m_handle.kind != CXCursorKind::CXCursor_NoDeclFound)
    {
        SourceFileDescriptor fileDesc(*this);
        const_cast<Cursor*>(this)->m_uid = fileDesc.GetHash();
        std::hash<unsigned> hasher;
        boost::hash_combine(const_cast<Cursor*>(this)->m_uid, hasher(fileDesc.line));
        boost::hash_combine(const_cast<Cursor*>(this)->m_uid, hasher(fileDesc.column));
    }
    return m_uid;
}

SourceFileDescriptor Cursor::GetFileDesc(void) const
{
    return SourceFileDescriptor(*this);
}

int Cursor::GetTemplateArgCnt() const
{
    return clang_Cursor_getNumTemplateArguments(m_handle);
}

SourceFileDescriptor::SourceFileDescriptor(Cursor const & cursor)
{
    //clang_getCursorLocation, clang_getCursorExtent
    auto range = clang_Cursor_getSpellingNameRange(cursor.m_handle, 0, 0);
    m_source_location = clang_getRangeStart(range);
    clang_getFileLocation(m_source_location, &file, &line, &column, &offset);
    clang_getFileUniqueID(file, &m_uid);

    int constexpr nelems = sizeof(m_uid.data) / sizeof(m_uid.data[0]);
    std::hash<unsigned long long> hasher;
    std::size_t h = hasher(m_uid.data[0]);
    for (int i = 1; i < nelems; ++i)
        boost::hash_combine(h, hasher(m_uid.data[i]));
    m_hash = h;
}

SourceFileDescriptor::SourceFileDescriptor(CXTranslationUnit tu, const char * file_name)
{
    CXFile file = clang_getFile(tu, file_name);
    clang_getFileUniqueID(file, &m_uid);

    int constexpr nelems = sizeof(m_uid.data) / sizeof(m_uid.data[0]);
    std::hash<unsigned long long> hasher;
    std::size_t h = hasher(m_uid.data[0]);
    for (int i = 1; i < nelems; ++i)
        boost::hash_combine(h, hasher(m_uid.data[i]));
    m_hash = h;
}

bool SourceFileDescriptor::InSystemHeader() const
{
    return clang_Location_isInSystemHeader(m_source_location);
}

bool SourceFileDescriptor::InCompiledSource() const
{
    return clang_Location_isFromMainFile(m_source_location);
}

std::size_t SourceFileDescriptor::GetHash(void) const
{
    return m_hash;
}

bool SourceFileDescriptor::IsEqual(SourceFileDescriptor const & rhs) const
{
    return std::equal(
        std::begin(m_uid.data),
        std::end(m_uid.data),
        std::begin(rhs.m_uid.data));
}
