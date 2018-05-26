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
#include "Reflection/Runtime/ReflectionModule.h"
#include "Core/ReflEx/AddressPack.h"
#include "Interfaces/INamespaceRegistry.h"
#include "AssemblyNameConfig.h"

namespace XamlCPP::Core::Proto
{
		class XamlNamespace : public Interfaces::Namespace
        {
            std::string _name;
            ReflEx::AddressPack addressPack;

        public:
            ReflEx::AddressPack const& GetAddressPack() const { return addressPack; }

            XamlNamespace(std::string const& name)
            {
                Name = name;
            }

            XamlNamespace(std::string const& name, ReflEx::AddressPack const&& addressPack)
                : addressPack(std::move(addressPack))
            {
                Name = name;
            }

            static std::vector<XamlNamespace*> DefinedInAssemblies(std::vector<System::Reflection::meta::ReflectionModule*> const& assemblies);

            static ReflEx::AssemblyNameConfig Map(std::string const& root);

            // Inherited via Namespace
            PROPERTY_T(std::string, Name)
            PROPERTY_T_GET(Name, ) override { return _name; };
            PROPERTY_T_SET(Name) override { _name = value; };

            virtual System::Reflection::Type const* Get(std::string const & typeName) override;

            bool operator<(XamlNamespace const& rhs)
            {
                return _name < rhs._name;
            }
        };
}

//End of file

