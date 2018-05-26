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
#include "Invokable.h"

class Class;

class MethodBase
    : public LanguageType
    , public Invokable
{
    friend class Class;
    friend class ReflectionModulParser;
    friend class ReflectionParser;

public:
    MethodBase(
        const Cursor &cursor,
        const Namespace &currentNamespace,
        Class *parent = nullptr
    );

    virtual ~MethodBase(void) {}

    std::string const& GetName() const { return m_name; }
    std::string const& GetDisplayName() const { return m_displayName; }

    virtual void MetaInit(const Cursor & cursor, const Namespace &currentNamespace) override;
    virtual bool ShouldCompile(void) const;

    bool IsStatic() const { return m_isStatic; }
    bool IsPureVirtual() const { return m_isPureVirtual; }
    bool IsTemplate() const { return m_isTemplate; }

    virtual void DeclareTemplate(CursorType const& defination) = 0;
    virtual void BuildTemplateDefinition() = 0;

    Mustache::Data<std::string> CompileStandardTemplate(Mustache::Data<std::string>& data, const ReflectionParser *context) const;

protected:
    bool m_isPureVirtual;
    bool m_isFunctionTemplate;
    bool m_isVirtual;
    bool m_isStatic;
    bool m_isTemplate;
    bool m_isReferenceReturnType;

    Class *m_parent = nullptr;

    std::string m_name;
    std::string m_displayName;

    std::vector<CursorType> m_templateTypes;
    std::vector<std::string> m_templateTypeNames;

    bool isAccessible(void) const;

    virtual std::string getQualifiedSignature(std::string const& type_name) const = 0;
};