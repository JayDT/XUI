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

#include "Interfaces/IXamlCPP.h"
#include "Runtime/ReflectionDatabase.h"
#include "Runtime/ReflectionModule.h"

#include "XamlNamespace.h"
#include "Proto/XamlNamespace.h"
#include "ReflEx/ConfiguredAssembly.h"
#include "ReflEx/ConfiguredAssemblyWithNamespaces.h"

std::vector<XamlCPP::Core::Proto::XamlNamespace*> XamlCPP::Core::Proto::XamlNamespace::DefinedInAssemblies(std::vector<System::Reflection::meta::ReflectionModule*> const & assemblies)
{
    std::vector<Proto::XamlNamespace*> xamlns;
    for (auto assembly : assemblies)
    {
        std::map<std::string, std::vector<XamlCPP::XmlnsDefinition const*>> xmlnsdefs;
        auto attributes = assembly->GetCustomAttributes<XamlCPP::XmlnsDefinition>();
        for (auto attr : attributes)
            xmlnsdefs[attr->XmlNamespace].push_back(attr);

        for (auto ns : xmlnsdefs)
        {
            std::vector<std::string> cxxnamespacies;
            for (auto attr : ns.second)
                cxxnamespacies.push_back(attr->ClrNamespace);

            ReflEx::AddressPack::AssemblyDataVec assembliesDataVec;

            ReflEx::ConfiguredAssembly* cAssembly = new ReflEx::ConfiguredAssembly(assembly);
            assembliesDataVec.push_back(cAssembly->WithNamespaces(cxxnamespacies));

            auto iXamlNS = std::find_if(xamlns.begin(), xamlns.end(), [ns](Proto::XamlNamespace* xns)
            {
                return ns.first == xns->Name;
            });

            if (iXamlNS != xamlns.end())
            {
                ((ReflEx::AddressPack&)(*iXamlNS)->GetAddressPack()).Append(cAssembly->WithNamespaces(cxxnamespacies));
            }
            else
            {
                ReflEx::AddressPack addressPack(assembliesDataVec);

                Proto::XamlNamespace* xns = new Proto::XamlNamespace(ns.first, std::move(addressPack));
                xamlns.push_back(xns);
            }
        }
    }
    return xamlns;
}

XamlCPP::Core::ReflEx::AssemblyNameConfig XamlCPP::Core::Proto::XamlNamespace::Map(std::string const& root)
{
    return ReflEx::AssemblyNameConfig(root);
}

System::Reflection::Type const* XamlCPP::Core::Proto::XamlNamespace::Get(std::string const & typeName)
{
    return addressPack.Get(typeName);
}
