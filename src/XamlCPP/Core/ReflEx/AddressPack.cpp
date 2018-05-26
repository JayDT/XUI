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
#include "AddressPack.h"
#include "ConfiguredAssemblyWithNamespaces.h"

XamlCPP::Core::ReflEx::AddressPack::AddressPack()
{
}

XamlCPP::Core::ReflEx::AddressPack::AddressPack(XamlCPP::Core::ReflEx::AddressPack::AssemblyDataVec const & assemblyAndClrs)
    : assembliesDataVec(assemblyAndClrs)
{
}

void XamlCPP::Core::ReflEx::AddressPack::Append(ConfiguredAssemblyWithNamespaces * cawn)
{
    assembliesDataVec.push_back(cawn);
}

const System::Reflection::Type * XamlCPP::Core::ReflEx::AddressPack::Get(std::string const & name) const
{
    for (auto item : assembliesDataVec)
    {
        auto ret = item->Get(name);
        if (ret)
            return ret;
    }
    return nullptr;
}

std::set<System::Reflection::meta::ReflectionModule*> XamlCPP::Core::ReflEx::AddressPack::GetUniqueAssemblies() const
{
    std::set<System::Reflection::meta::ReflectionModule*> ret;
    for (auto item : assembliesDataVec)
        ret.insert(item->Assembly);
    return ret;
}
