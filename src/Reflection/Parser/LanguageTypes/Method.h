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
class Method;

struct MethodOverridenDesc
{
	AstType* m_storageClassAstType;

	std::shared_ptr<Class> m_baseClass;
	std::shared_ptr<Method> m_baseMethod;
};

class Method 
    : public MethodBase
{
    friend class Class;
    friend class ReflectionModulParser;
    friend class ReflectionParser;

public:
    Method(
        const Cursor &cursor, 
        const Namespace &currentNamespace, 
        Class *parent = nullptr
    );

    virtual ~Method(void);

    void MetaInit(const Cursor & cursor, const Namespace &currentNamespace) override;
    bool ShouldCompile(void) const;

    void DeclareTemplate(CursorType const& defination) override;
    void BuildTemplateDefinition() override;

    Mustache::Data<std::string> CompileTemplate(
        const ReflectionParser *context
    ) const override;

private:
    bool m_isConst;

    template<typename T>
    using SharedPtrVector = std::list<std::shared_ptr<T>>;

    SharedPtrVector<Method> m_definations;
	std::vector<MethodOverridenDesc> m_overrided;

    bool isAccessible(void) const;

    std::string getQualifiedSignature(std::string const& type_name) const override;
};