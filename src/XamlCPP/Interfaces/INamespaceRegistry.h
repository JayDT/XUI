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
#include <set>

#include "Reflection/Runtime/Type.h"
#include "Core/Proto/PrefixRegistration.h"

namespace XamlCPP::Core::Proto
{
    class XamlNamespace;
}

namespace XamlCPP::Interfaces
{
    struct Namespace
    {
        virtual ~Namespace() {}
        PROPERTY_T(std::string, Name)
        virtual PROPERTY_T_GET(Name, ) = 0;
        virtual PROPERTY_T_SET(Name) = 0;
        virtual System::Reflection::Type const* Get(std::string const& typeName) = 0;
    };

    struct INamespaceRegistry
    {
        std::set<XamlCPP::Core::Proto::PrefixRegistration> RegisteredPrefixes;

        virtual ~INamespaceRegistry() {}
        virtual Namespace* GetNamespace(std::string const& name) = 0;
        virtual Namespace* GetNamespaceByPrefix(std::string const& prefix) = 0;
        virtual void RegisterPrefix(XamlCPP::Core::Proto::PrefixRegistration const& prefixRegistration) = 0;
        virtual void AddNamespace(XamlCPP::Core::Proto::XamlNamespace const& xamlNamespace) = 0;
        virtual void SetDefaultAssembly(const System::Reflection::meta::ReflectionModule *) = 0;
        virtual const System::Reflection::meta::ReflectionModule* GetDefaultAssembly() = 0;
    };
}
