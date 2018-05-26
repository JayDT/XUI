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
#include "ReflectionParser.h"
#include "Wrappers/Mustache.h"

class Class;
class Enum;
class ReflectionParser;

class LanguageType
{
    friend class ReflectionParser;

public:
    LanguageType();
    LanguageType(const Cursor &cursor, const Namespace &currentNamespace);
    virtual ~LanguageType(void) { }

    const MetaDataManager &GetMetaData(void) const;

    virtual void MetaInit(const Cursor &cursor, const Namespace &currentNamespace);
    std::string GetSourceFile(void) const;

    virtual Mustache::Data<std::string> CompileTemplate( const ReflectionParser *context ) const = 0;
    virtual bool ShouldCompile(void) const = 0;

    virtual Class* ToClass() { return nullptr; }
    virtual Enum* ToEnum() { return nullptr; }

protected:

    MetaDataManager m_metaData;

    // determines if the type is enabled in reflection database generation
    bool m_enabled;

    // determines if the pointer type to this type will be generated
    // in the reflection database
    bool m_ptrTypeEnabled;

    // determines if the constant pointer type to this type will be
    // generated in the reflection database
    bool m_constPtrTypeEnabled;

    // determines if this type generates data for its respective array type
    bool m_arrayTypeEnabled;

    CX_CXXAccessSpecifier m_accessModifier;
    CXVisibilityKind      m_visibility;

private:
    // cursor that represents the root of this language type
    Cursor m_rootCursor;
};