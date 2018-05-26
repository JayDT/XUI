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

#include "LanguageType.h"

class Class;
class MethodBase;
class ReflectionParser;
class ReflectionModulParser;

class FieldBase : public LanguageType
{
    friend class Class;
    friend class ReflectionParser;
    friend class ReflectionModulParser;

public:
    FieldBase(
        const Cursor &cursor,
        const Namespace &currentNamespace,
        Class *parent = nullptr
    );

    virtual ~FieldBase(void) { }

    virtual void MetaInit(const Cursor &cursor, const Namespace &currentNamespace) override;
    virtual bool ShouldCompile(void) const;
    Mustache::Data<std::string> CompileStandardTemplate(Mustache::Data<std::string>& data, const ReflectionParser *context) const;

    bool IsStatic() const { return m_isStatic; }
    bool IsProperty() const { return m_isProperty; }
    bool IsReference() const { return m_isReference; }
    bool IsTemplate() const { return m_isTemplate; }

    CursorType const& GetFieldType() const { return m_fieldType; }
    AstType* GetFieldAstType() const { return m_fieldAstType; }

    bool isMutable = false;
	std::shared_ptr<MethodBase> _getter_method;
	std::shared_ptr<MethodBase> _setter_method;

protected:
    bool m_isProperty;
    bool m_isConst;
    bool m_isReference;
    bool m_isPointer;
    bool m_isStatic;
    bool m_isConstSubType;
    bool m_isConstArray;
    bool m_isTemplate;
    bool m_NoDefaultClassCopyMethod = false;

    bool m_hasExplicitGetter;
    bool m_hasExplicitSetter;

    Class *m_parent;

    CursorType m_fieldType;
    AstType* m_fieldAstType;

    std::string m_name;
    std::string m_displayName;
    std::string m_type;
    std::string m_subType;

    std::string m_Getter;
    std::string m_Setter;

    std::string m_ExplicitGetter;
    std::string m_ExplicitSetter;

    size_t m_constArraySize;

    virtual bool isAccessible(void) const;
    virtual bool isGetterAccessible(void) const;
    virtual bool isSetterAccessible(void) const;
};