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

#include "LanguageTypes/MethodBase.h"

class Class;

class Function 
    : public MethodBase
{
    friend class ReflectionModulParser;
    friend class ReflectionParser;

public:
    Function(
        const Cursor &cursor, 
        const Namespace &currentNamespace, 
        Class *parent = nullptr
    );

    virtual ~Function(void) { }

    void MetaInit(const Cursor &cursor, const Namespace &currentNamespace) override;

    bool ShouldCompile(void) const;

    void DeclareTemplate(CursorType const& defination) override;
    void BuildTemplateDefinition() override;

    Mustache::Data<std::string> CompileTemplate(
        const ReflectionParser *context
    ) const override;

private:

    template<typename T>
    using SharedPtrVector = std::list<std::shared_ptr<T>>;

    SharedPtrVector<Function> m_definations;

    bool isAccessible(void) const;

    std::string getQualifiedSignature(std::string const& type_name) const override;
};