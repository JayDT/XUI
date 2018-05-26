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
#ifndef __XAMLCPP_RUNTIME_TYPE_SOURCE_H__
#define __XAMLCPP_RUNTIME_TYPE_SOURCE_H__

#include <System/System.h>
#include "XamlType.h"
#include "Interfaces/IRuntimeTypeSource.h"
#include "Interfaces/INamespaceRegistry.h"

namespace XamlCPP::Core
{
    namespace Proto
    {
        class PrefixRegistration;
        class XamlNamespace;
        class XamlType;
    };

    namespace ReflEx
    {
        class AttachableMember;
        class Member;
    }

    class RuntimeTypeSource : public Interfaces::IRuntimeTypeSource
    {
    private:
        Interfaces::INamespaceRegistry *privateNamespaceRegistry;
        Interfaces::ITypeRepository *privateTypeRepository;

    public:
        Interfaces::INamespaceRegistry *getNamespaceRegistry() const;

        RuntimeTypeSource(Interfaces::ITypeRepository *typeRepository, Interfaces::INamespaceRegistry *nsRegistry);

        Interfaces::Namespace *GetNamespace(const std::string &name) override;

        Interfaces::Namespace *GetNamespaceByPrefix(const std::string &prefix) override;

        void RegisterPrefix(Proto::PrefixRegistration const& prefixRegistration) override;

        void AddNamespace(Proto::XamlNamespace const& xamlNamespace) override;

        Proto::XamlType *GetByType(System::Reflection::Type const*type) override;

        Proto::XamlType *GetByQualifiedName(const std::string &qualifiedName) override;

        Proto::XamlType *GetByPrefix(const std::string &prefix, const std::string &typeName) override;

        Proto::XamlType *GetByFullAddress(Proto::XamlTypeName *xamlTypeName) override;

        ReflEx::Member *GetMember(System::Reflection::meta::Field const*propertyInfo) override;

        ReflEx::AttachableMember *GetAttachableMember(const std::string &name, const System::Reflection::IMethod* getter, const System::Reflection::IMethod* setter) override;

        std::set<XamlCPP::Core::Proto::PrefixRegistration> const& getRegisteredPrefixes() const;

        Interfaces::ITypeRepository *getTypeRepository() const;

        static std::shared_ptr<Interfaces::IRuntimeTypeSource> FromAttributes(std::vector<System::Reflection::meta::ReflectionModule*> &assemblies);

        void SetDefaultAssembly(const System::Reflection::meta::ReflectionModule *assembly) override
        {
            privateNamespaceRegistry->SetDefaultAssembly(assembly);
        }

        const System::Reflection::meta::ReflectionModule* GetDefaultAssembly() override { return privateNamespaceRegistry->GetDefaultAssembly(); }

        void AddTypeConverter(Core::TypeConverterRegistration const& typeConverterRegistration) override;

        virtual Interfaces::INamespaceRegistry * GetNamespaceScope() const override { return privateNamespaceRegistry; }
    };
}

#endif // !__XAMLCPP_RUNTIME_TYPE_SOURCE_H__
