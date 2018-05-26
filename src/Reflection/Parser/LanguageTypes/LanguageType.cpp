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

#include "LanguageType.h"
#include "MetaDataConfig.h"

LanguageType::LanguageType()
    : m_enabled(false)
    , m_ptrTypeEnabled(true)
    , m_constPtrTypeEnabled(true)
    , m_arrayTypeEnabled(true)
    , m_accessModifier(CX_CXXAccessSpecifier::CX_CXXPublic)
    , m_rootCursor()
{
}

LanguageType::LanguageType(
    const Cursor &cursor, 
    const Namespace &currentNamespace
)
    : m_metaData( )
    , m_enabled(false)
    , m_ptrTypeEnabled(true)
    , m_constPtrTypeEnabled(true)
    , m_arrayTypeEnabled(true)
    , m_accessModifier( cursor.GetAccessModifier( ) )
    , m_rootCursor( cursor )
{
}

const MetaDataManager &LanguageType::GetMetaData(void) const
{
    return m_metaData;
}

void LanguageType::MetaInit(const Cursor & cursor, const Namespace & currentNamespace)
{
    m_enabled = (m_metaData.GetFlag(native_attribute::Enable));
    m_ptrTypeEnabled = (m_metaData.GetFlag(native_attribute::EnablePtrType));
    m_constPtrTypeEnabled = (m_metaData.GetFlag(native_attribute::EnableConstPtrType));
    m_arrayTypeEnabled = (m_metaData.GetFlag(native_attribute::EnableArrayType));
    m_visibility = CXVisibilityKind(cursor.Query(clang_getCursorVisibility));
}

std::string LanguageType::GetSourceFile(void) const
{
    return m_rootCursor.GetSourceFile( );
}