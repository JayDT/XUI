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

#include "Interfaces/IXamlCPP.h"
#include "RuntimeTypeSource.h"
#include "Runtime/ReflectionDatabase.h"
#include "Runtime/ReflectionModule.h"

#include "Proto/XamlNamespace.h"
#include "ReflEx/ConfiguredAssembly.h"
#include "ReflEx/ConfiguredAssemblyWithNamespaces.h"
#include "ReflEx/NamespaceRegistry.h"
#include "ReflEx/TypeFactory.h"
#include "ReflEx/TypeConverterProvider.h"
#include "ReflEx/TypeFeatureProvider.h"
#include "Default/TypeRepository.h"

#include "cpplinq/linq.hpp"

using namespace XamlCPP::Core;
using namespace XamlCPP::Core::Proto;

XamlCPP::Interfaces::INamespaceRegistry *RuntimeTypeSource::getNamespaceRegistry() const
{
    return privateNamespaceRegistry;
}

RuntimeTypeSource::RuntimeTypeSource(ITypeRepository *typeRepository, INamespaceRegistry *nsRegistry)
{
    this->privateTypeRepository = typeRepository;
    this->privateNamespaceRegistry = nsRegistry;
}

::XamlCPP::Interfaces::Namespace *RuntimeTypeSource::GetNamespace(const std::string &name)
{
    return getNamespaceRegistry()->GetNamespace(name);
}

::XamlCPP::Interfaces::Namespace *RuntimeTypeSource::GetNamespaceByPrefix(const std::string &prefix)
{
    return getNamespaceRegistry()->GetNamespaceByPrefix(prefix);
}

void RuntimeTypeSource::RegisterPrefix(PrefixRegistration const& prefixRegistration)
{
    getNamespaceRegistry()->RegisterPrefix(prefixRegistration);
}

void RuntimeTypeSource::AddNamespace(XamlNamespace const& xamlNamespace)
{
    getNamespaceRegistry()->AddNamespace(xamlNamespace);
}

XamlType *RuntimeTypeSource::GetByType(System::Reflection::Type const*type)
{
    return getTypeRepository()->GetByType(type);
}

XamlType *RuntimeTypeSource::GetByQualifiedName(const std::string &qualifiedName)
{
    return getTypeRepository()->GetByQualifiedName(qualifiedName);
}

XamlType *RuntimeTypeSource::GetByPrefix(const std::string &prefix, const std::string &typeName)
{
    return getTypeRepository()->GetByPrefix(prefix, typeName);
}

XamlType *RuntimeTypeSource::GetByFullAddress(XamlTypeName *xamlTypeName)
{
    return getTypeRepository()->GetByFullAddress(xamlTypeName);
}

ReflEx::Member *RuntimeTypeSource::GetMember(System::Reflection::meta::Field const*propertyInfo)
{
    return getTypeRepository()->GetMember(propertyInfo);
}

ReflEx::AttachableMember *RuntimeTypeSource::GetAttachableMember(const std::string &name, const System::Reflection::IMethod* getter, const System::Reflection::IMethod* setter)
{
    return getTypeRepository()->GetAttachableMember(name, getter, setter);
}

std::set<XamlCPP::Core::Proto::PrefixRegistration> const& RuntimeTypeSource::getRegisteredPrefixes() const
{
    return getNamespaceRegistry()->RegisteredPrefixes;
}

XamlCPP::Interfaces::ITypeRepository *RuntimeTypeSource::getTypeRepository() const
{
    return privateTypeRepository;
}

std::shared_ptr<XamlCPP::Interfaces::IRuntimeTypeSource> RuntimeTypeSource::FromAttributes(std::vector<System::Reflection::meta::ReflectionModule*> &assemblies)
{
    auto xamlNamespaceRegistry = new ReflEx::NamespaceRegistry();
    xamlNamespaceRegistry->FillFromAttributes(assemblies);
    
    auto typeFactory = new ReflEx::TypeFactory();

    std::set<System::Reflection::Type const*> allExportedTypes;
    for (auto xamlNs : xamlNamespaceRegistry->GetXamlNamespaces())
    {
        for (auto assembly : xamlNs->GetAddressPack().GetUniqueAssemblies())
        {
            for (System::Reflection::Type const* type : assembly->GetTypes())
            {
                if (type->GetVisibility() == System::Reflection::meta::CXVisibilityKind::CXVisibility_Default)
                {
                    allExportedTypes.insert(type);
                }
            }
        }
    }

    auto typeFeatureProvider = new ReflEx::TypeFeatureProvider(std::make_shared<ReflEx::TypeConverterProvider>());
    typeFeatureProvider->FillFromAttributes(allExportedTypes);

    auto xamlTypeRepo = new TypeRepository(xamlNamespaceRegistry, typeFactory, typeFeatureProvider);
    return std::make_shared<RuntimeTypeSource>(xamlTypeRepo, xamlNamespaceRegistry);
}

void XamlCPP::Core::RuntimeTypeSource::AddTypeConverter(Core::TypeConverterRegistration const & typeConverterRegistration)
{
    privateTypeRepository->AddTypeConverter(typeConverterRegistration);
}
