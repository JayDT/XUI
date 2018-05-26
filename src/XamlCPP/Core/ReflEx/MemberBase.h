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
#include <System/System.h>
#include "XamlType.h"
#include <standard/collection.h>

namespace XamlCPP::Core::ReflEx
{
    class MutableMember;
    class AttachableMember;

    struct
        Meta(ReflInclude("Core/ReflEx/AttachableMember.h"))
        MemberBase : System::Reflection::Object
    {
        friend class System::Reflection::Assembly::AssemblyModulXamlCPP;

        std::string Name;
        Proto::XamlType* XamlType;
        virtual bool IsDirective() const = 0;
        virtual bool IsAttachable() const = 0;
        virtual MutableMember* ToMutableMember() { return nullptr; }
        virtual AttachableMember* ToAttachableMember() { return nullptr; }

        std::string ToString() const override
        {
            return "MemberBase " + Name;
        }

    protected:
        MemberBase(std::string const& name)
        {
            Name = name;
        }
    };
}
