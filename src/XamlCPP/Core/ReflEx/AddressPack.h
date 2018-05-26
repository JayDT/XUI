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

//File: AddressPack.h

#pragma once
#include <System/System.h>
#include "Reflection/Runtime/Type.h"

namespace XamlCPP::Core::ReflEx
{
    class ConfiguredAssemblyWithNamespaces;

    class AddressPack
    {
    public:
        typedef std::vector<ConfiguredAssemblyWithNamespaces*> AssemblyDataVec;

    private:
        AssemblyDataVec assembliesDataVec;

    public:
        AddressPack();
        AddressPack(AssemblyDataVec const& assemblyAndClrs);
        void Append(ConfiguredAssemblyWithNamespaces*);
        const System::Reflection::Type* Get(std::string const& name) const;
        std::set<System::Reflection::meta::ReflectionModule*> GetUniqueAssemblies() const;
    };
}

//End of file

