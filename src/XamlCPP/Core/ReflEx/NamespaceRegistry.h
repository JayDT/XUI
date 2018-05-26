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
#ifndef __XAMLCPP_NAMESPACEREGISTRY_H__
#define __XAMLCPP_NAMESPACEREGISTRY_H__

#include <System/System.h>
#include "Interfaces/INamespaceRegistry.h"

namespace XamlCPP::Core::Proto
{
    class PrefixRegistration;
    class XamlNamespace;
};


namespace XamlCPP::Core::ReflEx
{
    class ClrNamespace;

    class NamespaceRegistry : public Interfaces::INamespaceRegistry
    {
    private:
        static const char* ClrNamespace;
        std::unordered_map<std::string, std::string> registeredPrefixes;
        std::set<Proto::XamlNamespace*> xamlNamespaces;
        std::unordered_map<std::string, ReflEx::ClrNamespace*> clrNamespaces;
        std::vector<Proto::PrefixRegistration> prefixRegistrations;
        const System::Reflection::meta::ReflectionModule * m_defaultAssembly = nullptr;
    public:
        virtual ~NamespaceRegistry()
        {
        }

        std::vector<Proto::PrefixRegistration> const& getRegisteredPrefixes() const;
        Interfaces::Namespace *GetNamespaceByPrefix(const std::string &prefix);
    private:
        std::string GetXamlNamespaceNameByPrefix(const std::string &prefix);
    public:
        void RegisterPrefix(Proto::PrefixRegistration const& prefixRegistration) override;

    private:
        void RegisterWhenItsClrNs(const std::string &prefix, const std::string &namespace_);

        static ReflEx::ClrNamespace *ExtractNamespace(const std::string &formattedClrString, const System::Reflection::meta::ReflectionModule * defaultAssembly);
        static System::Reflection::meta::ReflectionModule const* GetAssembly(const std::string &assemblyPart);
        static bool IsClrNamespace(const std::string &ns);
    public:
        Proto::XamlNamespace *GetXamlNamespace(const std::string &ns);
        Interfaces::Namespace *GetNamespace(const std::string &name);
        Proto::XamlNamespace *GetXamlNamespaceByPrefix(const std::string &prefix);
        void AddNamespace(Proto::XamlNamespace const& xamlNamespace) override;
        ReflEx::ClrNamespace *GetClrNamespaceByPrefix(const std::string &prefix);

        void FillFromAttributes(std::vector<System::Reflection::meta::ReflectionModule*> &assemblies);
        std::set<Proto::XamlNamespace*> const& GetXamlNamespaces() const { return xamlNamespaces; }
        void SetDefaultAssembly(const System::Reflection::meta::ReflectionModule *assembly) override
        {
            m_defaultAssembly = assembly;
        }

        const System::Reflection::meta::ReflectionModule* GetDefaultAssembly() override { return m_defaultAssembly; }
    };
}

#endif // !__XAMLCPP_NAMESPACEREGISTRY_H__
