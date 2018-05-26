/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 OmniGUI Platform
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

#include "StdAfx.h"
#include "CoreTypes.h"
#include "cpplinq/linq.hpp"

using namespace XamlCPP::Core;

struct DirectiveDesc
{
    XamlCPP::Core::Assembler::Commands::DirectiveKind Kind;
    ReflEx::Directive Directive;
};

std::vector<DirectiveDesc>& getDirectives()
{
    static std::vector<DirectiveDesc> SupportedDirective =
    {
        // Custom
        { XamlCPP::Core::Assembler::Commands::DirectiveKind::Items, { "_Items", XamlCPP::Core::Proto::XamlType::CreateForBuiltInType(&typeof(System::Collection::List<System::Reflection::RefObject>)) } },
        { XamlCPP::Core::Assembler::Commands::DirectiveKind::Initialization, "_Initialization" },
        { XamlCPP::Core::Assembler::Commands::DirectiveKind::MarkupExtensionArguments, "_MarkupExtensionParameters" },
        { XamlCPP::Core::Assembler::Commands::DirectiveKind::UnknownContent, "_UnknownContent" },
        // Wpf
        { XamlCPP::Core::Assembler::Commands::DirectiveKind::Key, "Key" },
        { XamlCPP::Core::Assembler::Commands::DirectiveKind::Name, "Name" },
        { XamlCPP::Core::Assembler::Commands::DirectiveKind::Class, "Class" },
        { XamlCPP::Core::Assembler::Commands::DirectiveKind::UID, "Uid" },
        { XamlCPP::Core::Assembler::Commands::DirectiveKind::Code, "Code" },
    };

    return SupportedDirective;
}

ReflEx::Directive * XamlCPP::Core::CoreTypes::getItems()
{
    return GetDirective("_Items");
}

ReflEx::Directive * XamlCPP::Core::CoreTypes::getUnknownContent()
{
    return GetDirective("_UnknownContent");
}

ReflEx::Directive * XamlCPP::Core::CoreTypes::getInitialization()
{
    return GetDirective("_Initialization");
}

ReflEx::Directive * XamlCPP::Core::CoreTypes::getMarkupExtensionArguments()
{
    return GetDirective("_MarkupExtensionParameters");
}

ReflEx::Directive * XamlCPP::Core::CoreTypes::getName()
{
    return GetDirective("Name");
}

ReflEx::Directive * XamlCPP::Core::CoreTypes::getKey()
{
    return GetDirective("Key");
}

Proto::XamlType *CoreTypes::getString()
{
    return Proto::XamlType::CreateForBuiltInType(&typeof(std::string));
}

Proto::XamlType *CoreTypes::getInt32()
{
    return Proto::XamlType::CreateForBuiltInType(&typeof(int));
}

ReflEx::Directive *CoreTypes::GetDirective(const std::string &name)
{
    for (auto& directive : getDirectives())
    {
        if (directive.Directive.Name == name)
            return &directive.Directive;
    }

    return getUnknownContent();
}

XamlCPP::Core::Assembler::Commands::DirectiveKind XamlCPP::Core::CoreTypes::GetDirectiveKind(const std::string & name)
{
    for (auto& directive : getDirectives())
    {
        if (directive.Directive.Name == name)
            return directive.Kind;
    }

    return XamlCPP::Core::Assembler::Commands::DirectiveKind::UnknownContent;
}

XamlCPP::Core::Assembler::Commands::DirectiveKind XamlCPP::Core::CoreTypes::GetDirectiveKind(const ReflEx::Directive * _directive)
{
    for (auto& directive : getDirectives())
    {
        if ((&directive.Directive) == _directive)
            return directive.Kind;
    }

    return XamlCPP::Core::Assembler::Commands::DirectiveKind::UnknownContent;
}

