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
#include "LanguageTypes/Class.h"
#include "LanguageTypes/Field.h"
#include "Runtime/MetaProperty.h"
#include "MetaDataConfig.h"
#include "MetaUtils.h"
#include "ReflectionParser.h"

#include "standard/Platform/Define.h"

extern unsigned long internalID;

Field::Field(
    const Cursor &cursor,
    const Namespace &currentNamespace,
    Class *parent
)
    : FieldBase(cursor, currentNamespace, parent)
    , m_isConst(cursor.GetType().IsConst())
{
    isMutable = cursor.Query(clang_CXXField_isMutable);
}

void Field::MetaInit(const Cursor & cursor, const Namespace & currentNamespace)
{
    FieldBase::MetaInit(cursor, currentNamespace);
}

bool Field::ShouldCompile(void) const
{
    if (m_type.empty())
        return false;
    return isAccessible();
}

Mustache::Data<std::string> Field::CompileTemplate(const ReflectionParser *context) const
{
    Mustache::Data<std::string> data = { Mustache::Data<std::string>::Type::Object };

    FieldBase::CompileStandardTemplate(data, context);

    return data;
}

bool Field::isAccessible(void) const
{
    return (m_hasExplicitGetter || m_hasExplicitSetter) || FieldBase::isAccessible();
}

bool Field::isGetterAccessible(void) const
{
    return FieldBase::isGetterAccessible();
}

bool Field::isSetterAccessible(void) const
{
    return FieldBase::isSetterAccessible()/*&& (!m_isConst || isMutable)*/;
}