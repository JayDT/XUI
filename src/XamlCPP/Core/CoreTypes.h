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

#pragma once

#include <string>
#include <vector>

#include "Proto/XamlType.h"
#include "ReflEx/Directive.h"
#include "Core/Assembler/Command/StartMemberCommand.h"

namespace XamlCPP::Core
{
    namespace ReflEx
    {
        struct Directive;
        struct MemberBase;
        class NamespaceDeclaration;
    };

    class CoreTypes final
    {
    public:
        static constexpr const char* SpecialNamespace[] =
        {
            "http://git.tauri.hu/Xaml",
            "http://schemas.microsoft.com/winfx/2006/xaml",
            "http://schemas.microsoft.com/winfx/2006/xaml/presentation",
            "http://schemas.microsoft.com/expression/blend/2008",
            "http://schemas.openxmlformats.org/markup-compatibility/2006"
        };

        static ReflEx::Directive *getItems();
        static ReflEx::Directive *getUnknownContent();
        static ReflEx::Directive *getInitialization();
        static ReflEx::Directive *getMarkupExtensionArguments();
        static ReflEx::Directive *getName();
        static ReflEx::Directive *getKey();

        static Proto::XamlType *getString();
        static Proto::XamlType *getInt32();

        static ReflEx::Directive *GetDirective(const std::string &name);
        static XamlCPP::Core::Assembler::Commands::DirectiveKind GetDirectiveKind(const std::string &name);
        static XamlCPP::Core::Assembler::Commands::DirectiveKind GetDirectiveKind(const ReflEx::Directive *directive);
    };
}