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
#include "TypeRepository.h"
#include "Interfaces/IXamlCPP.h"
#include "Core/Proto/XamlType.h"
#include "Core/Proto/XamlTypeName.h"
#include "Core/ReflEx/Member.h"
#include "Core/ReflEx/AttachableMember.h"

using namespace XamlCPP;
using namespace XamlCPP::Core;

XamlCPP::Core::TypeRepository::TypeRepository(Interfaces::INamespaceRegistry * namespaceRegistry, Interfaces::ITypeFactory * typeTypeFactory, Interfaces::ITypeFeatureProvider * featureProvider)
    : namespaceRegistry(namespaceRegistry), typeTypeFactory(typeTypeFactory), featureProvider(featureProvider)
{
    System::ThrowIfFailed<System::ArgumentException>(namespaceRegistry, "namespaceRegistry");
    System::ThrowIfFailed<System::ArgumentException>(typeTypeFactory, "typeTypeFactory");
    System::ThrowIfFailed<System::ArgumentException>(featureProvider, "featureProvider");
}

Interfaces::ITypeFeatureProvider * XamlCPP::Core::TypeRepository::getFeatureProvider() const
{
    return featureProvider;
}

Interfaces::ITypeFactory * XamlCPP::Core::TypeRepository::getTypeFactory() const
{
    return typeTypeFactory;
}

Proto::XamlType * XamlCPP::Core::TypeRepository::GetByType(System::Reflection::Type const * type)
{
    System::ThrowIfFailed<System::ArgumentException>(type && type->IsValid(), "type");

    auto iType = XamlTypeMap.find(type);
    if (iType != XamlTypeMap.end())
        return iType->second;

    auto xamlType = Proto::XamlType::Create(type, this, getTypeFactory(), featureProvider);
    XamlTypeMap[type] = xamlType;
    return xamlType;
}

Proto::XamlType * XamlCPP::Core::TypeRepository::GetByQualifiedName(const std::string & qualifiedName)
{
    auto tuple = System::String(qualifiedName).Split(':');

    auto prefix = tuple.size() <= 1 ? "" : tuple[0];
    auto typeName = tuple.size() > 1 ? tuple[1] : tuple[0];

    return GetByPrefix(prefix, typeName);
}

Proto::XamlType * XamlCPP::Core::TypeRepository::GetByPrefix(const std::string & prefix, const std::string & typeName)
{
    auto ns = namespaceRegistry->GetNamespaceByPrefix(prefix);

    if (ns == nullptr)
    {
        throw ParseException(System::String::format("Cannot find a namespace with the prefix \"{0}\"", prefix));
    }

    auto type = ns->Get(typeName);

    if (!type || !type->IsValid())
    {
        throw ParseException(System::String::format("The type \"{{{0}:{1}}} cannot be found\"", prefix, typeName));
    }

    return GetByType(type);
}

Proto::XamlType * XamlCPP::Core::TypeRepository::GetByFullAddress(Proto::XamlTypeName * xamlTypeName)
{
    auto ns = namespaceRegistry->GetNamespace(xamlTypeName->Namespace);

    if (ns == nullptr)
    {
        throw TypeNotFoundException(System::String::format("Error trying to resolve a XamlType: Cannot find the namespace '{0}'", xamlTypeName->Namespace));
    }

    auto correspondingType = ns->Get(xamlTypeName->Name);

    if (correspondingType != nullptr)
    {
        return GetByType(correspondingType);
    }

    throw TypeNotFoundException(System::String::format("Error trying to resolve a XamlType: The type {0} has not been found into the namespace '{1}'", xamlTypeName->Name, xamlTypeName->Namespace));
}

ReflEx::Member * XamlCPP::Core::TypeRepository::GetMember(System::Reflection::meta::Field const * propertyInfo)
{
    auto owner = GetByType(&propertyInfo->DeclaringType());
    return new ReflEx::Member(propertyInfo->GetName(), owner, this, featureProvider);
}

ReflEx::AttachableMember * XamlCPP::Core::TypeRepository::GetAttachableMember(const std::string & name, const System::Reflection::IMethod* getter, const System::Reflection::IMethod* setter)
{
    return new ReflEx::AttachableMember(name, getter, setter, this, featureProvider);
}

void XamlCPP::Core::TypeRepository::AddTypeConverter(TypeConverterRegistration const & typeConverterRegistration)
{
    featureProvider->AddTypeConverter(typeConverterRegistration);
}
