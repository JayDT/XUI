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

#include "LanguageTypes/Global.h"

class ReflectionParser;

class Enum : public LanguageType
{
    friend class ReflectionParser;

public:
    struct Value
    {
        Value(Enum *parent, const Cursor &cursor);

        std::string key;
        std::string value;
    };

    friend struct Value;

    Enum(const Cursor &cursor, const Namespace &currentNamespace);
    virtual ~Enum(void) { }

    void MetaInit(const Cursor &cursor, const Namespace &currentNamespace) override;
    bool ShouldCompile(void) const;
    virtual Enum* ToEnum() override final { return this; }
    
    Mustache::Data<std::string> CompileTemplate(
        const ReflectionParser *context
    ) const override;

private:
    std::string m_name;
    std::string m_displayName;
    std::string m_qualifiedName;

    std::vector<Value> m_values;

    bool isAccessible(void) const;
};