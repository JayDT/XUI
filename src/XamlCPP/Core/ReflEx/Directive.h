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
#include "MemberBase.h"

namespace XamlCPP::Core::ReflEx
{
    struct Directive : public MemberBase
    {
        friend class System::Reflection::Assembly::AssemblyModulXamlCPP;

        Directive() : MemberBase("") {}
        Directive(std::string const& name);
        Directive(std::string const& name, Proto::XamlType* xamlType);

        // This fake method for reflection definitate type for generic _Items directive! (need implement Meta(RefTypeDecl(System::Collection::List<System::Reflection::RefObject>)))
#if defined(__REFLECTION_PARSER__)
        Meta(Disable) System::Collection::List<System::Reflection::RefObject> _() {}
#endif

        // Inherited via MemberBase
        virtual bool IsDirective() const override;
        virtual bool IsAttachable() const override;

        std::string ToString() const override
        {
            return "Directive: " + Name;
        }
    };
}
