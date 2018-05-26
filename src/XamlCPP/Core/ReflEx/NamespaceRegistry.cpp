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
#include "NamespaceRegistry.h"
#include "Proto/XamlNamespace.h"
#include "Proto/PrefixRegistration.h"
#include "ReflEx/ClrNamespace.h"
#include "standard/Errors.h"

#include <boost/algorithm/string/predicate.hpp>
#include "cpplinq/linq.hpp"

using namespace XamlCPP;
using namespace XamlCPP::Core;

const char* XamlCPP::Core::ReflEx::NamespaceRegistry::ClrNamespace = "clr-namespace:";

std::vector<Proto::PrefixRegistration> const& XamlCPP::Core::ReflEx::NamespaceRegistry::getRegisteredPrefixes() const
{
    return prefixRegistrations;
}

Interfaces::Namespace* XamlCPP::Core::ReflEx::NamespaceRegistry::GetNamespaceByPrefix(const std::string & prefix)
{
    ReflEx::ClrNamespace *clrNs;
    auto clrNamespaces_iterator = clrNamespaces.find(prefix);
    auto isClrNs = clrNamespaces_iterator != clrNamespaces.end();
    if (isClrNs)
    {
        clrNs = clrNamespaces_iterator->second;
        return static_cast<Interfaces::Namespace*>(clrNs);
    }
    auto namespaceName = GetXamlNamespaceNameByPrefix(prefix);
    return static_cast<Interfaces::Namespace*>(GetXamlNamespace(namespaceName));
}

std::string XamlCPP::Core::ReflEx::NamespaceRegistry::GetXamlNamespaceNameByPrefix(const std::string & prefix)
{
    bool isFound;
    std::string name;
    auto registeredPrefixes_iterator = registeredPrefixes.find(prefix);
    isFound = registeredPrefixes_iterator != registeredPrefixes.end();
    name = registeredPrefixes_iterator->second;
    if (!isFound)
    {
        throw System::KeyNotFoundException(System::String::format("The prefix %s is not registered", prefix.c_str()).c_str());
    }
    return name;
}

void XamlCPP::Core::ReflEx::NamespaceRegistry::RegisterPrefix(Proto::PrefixRegistration const& prefixRegistration)
{
    if (registeredPrefixes.find(prefixRegistration.getPrefix()) == registeredPrefixes.end())
    {
        auto prefix = prefixRegistration.getPrefix();
        auto namespace_ = prefixRegistration.getNs();

        RegisterWhenItsClrNs(prefix, namespace_);

        prefixRegistrations.emplace_back(std::move(prefixRegistration));
        registeredPrefixes.emplace(prefixRegistration.getPrefix(), namespace_);
    }
}

void XamlCPP::Core::ReflEx::NamespaceRegistry::RegisterWhenItsClrNs(const std::string & prefix, const std::string & namespace_)
{
    if (IsClrNamespace(namespace_))
    {
        clrNamespaces.emplace(prefix, ExtractNamespace(namespace_, m_defaultAssembly));
    }
}

ReflEx::ClrNamespace * XamlCPP::Core::ReflEx::NamespaceRegistry::ExtractNamespace(const std::string & formattedClrString, const System::Reflection::meta::ReflectionModule * defaultAssembly)
{
    auto startOfNamespace = (int)formattedClrString.find(":") + 1;
    auto endOfNamespace = (int)formattedClrString.find(";");

    if (endOfNamespace < 0)
    {
        endOfNamespace = formattedClrString.length() - startOfNamespace;
    }

    auto ns = formattedClrString.substr(startOfNamespace, endOfNamespace - startOfNamespace);

    auto remainingPartStart = startOfNamespace + ns.length() + 1;
    auto remainingPartLenght = (int)formattedClrString.length() - (int)remainingPartStart;

    System::Reflection::meta::ReflectionModule const * assembly = nullptr;
    if (remainingPartLenght > 0)
    {
        auto assemblyPart = formattedClrString.substr(remainingPartStart, remainingPartLenght);

        assembly = GetAssembly(assemblyPart);
    }
    else if (defaultAssembly)
    {
        assembly = defaultAssembly;
    }
    else
        return nullptr;

    return new ReflEx::ClrNamespace((System::Reflection::meta::ReflectionModule *)assembly, ns);
}

System::Reflection::meta::ReflectionModule const * XamlCPP::Core::ReflEx::NamespaceRegistry::GetAssembly(const std::string & assemblyPart)
{
    auto dicotomize = System::String(assemblyPart).Split('=');
    if (dicotomize.size() > 1 && dicotomize[0] == "assembly")
    {
        return System::Reflection::meta::ReflectionModulManager::Instance().GetAssembly(dicotomize[1]);
    }
    return nullptr;
}

bool XamlCPP::Core::ReflEx::NamespaceRegistry::IsClrNamespace(const std::string & ns)
{
    return boost::starts_with(ns, ClrNamespace);
}

Proto::XamlNamespace * XamlCPP::Core::ReflEx::NamespaceRegistry::GetXamlNamespace(const std::string & ns)
{
    auto xamlNamespace = cpplinq::from(xamlNamespaces).first_or_default([&](Proto::XamlNamespace* ns1)
    {
        return ns1->Name == ns;
    });
    return xamlNamespace;
}

Interfaces::Namespace * XamlCPP::Core::ReflEx::NamespaceRegistry::GetNamespace(const std::string & name)
{
    return cpplinq::from(xamlNamespaces).first_or_default([&](Proto::XamlNamespace* xamlNamespace)
    {
        return xamlNamespace->Name == name;
    });
}

Proto::XamlNamespace * XamlCPP::Core::ReflEx::NamespaceRegistry::GetXamlNamespaceByPrefix(const std::string & prefix)
{
    return GetXamlNamespace(registeredPrefixes[prefix]);
}

void XamlCPP::Core::ReflEx::NamespaceRegistry::AddNamespace(Proto::XamlNamespace const& xamlNamespace)
{
    xamlNamespaces.insert((Proto::XamlNamespace*)&xamlNamespace);
}

ReflEx::ClrNamespace * XamlCPP::Core::ReflEx::NamespaceRegistry::GetClrNamespaceByPrefix(const std::string & prefix)
{
    return clrNamespaces[prefix];
}

void XamlCPP::Core::ReflEx::NamespaceRegistry::FillFromAttributes(std::vector<System::Reflection::meta::ReflectionModule*>& assemblies)
{
    auto namespaces = Proto::XamlNamespace::DefinedInAssemblies(assemblies);

    for (auto const& xamlNamespace : namespaces)
    {
        AddNamespace(*xamlNamespace);
    }
}
