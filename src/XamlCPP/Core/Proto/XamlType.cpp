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

#include "XamlType.h"
#include "Metadata.h"
#include "ReflEx/Member.h"
#include "ReflEx/AttachableMember.h"

using namespace XamlCPP;
using namespace XamlCPP::Core;
using namespace XamlCPP::Core::Proto;

ReflEx::Member * XamlCPP::Core::Proto::XamlType::getContentProperty()
{
    auto propertyName = featureProvider->GetContentPropertyName(underlyingType);

    if (propertyName.empty())
        return nullptr;

    auto type = typeRepository->GetByType(underlyingType);
    if (!type)
        return nullptr;

    auto member = type->GetMember(propertyName);
    //delete type;
    return member;
}

Interfaces::ITypeConverter * XamlCPP::Core::Proto::XamlType::getTypeConverter()
{
    return featureProvider->GetTypeConverter(underlyingType->GetHashCode());
}

ReflEx::Member * XamlCPP::Core::Proto::XamlType::getRuntimeNamePropertyMember()
{
    if (typeRepository == nullptr)
        return nullptr;

    std::shared_ptr<XamlCPP::Core::Metadata> metadata = featureProvider->GetMetadata(underlyingType);

    auto runtimeNameProperty = metadata ? metadata->RuntimePropertyName : std::string();

    if (runtimeNameProperty.empty())
        return nullptr;

    auto const& propInfo = underlyingType->GetRuntimeProperty(runtimeNameProperty);
    if (!propInfo.IsValid())
        return nullptr;

    return GetMember(runtimeNameProperty);
}

ReflEx::Member * XamlCPP::Core::Proto::XamlType::GetMember(std::string const & name)
{
    return LookupMember(name);
}

ReflEx::Member * XamlCPP::Core::Proto::XamlType::LookupMember(std::string const & name)
{
    return new ReflEx::Member(name, this, typeRepository, featureProvider);
}

ReflEx::AttachableMember * XamlCPP::Core::Proto::XamlType::GetAttachableMember(std::string const & name)
{
    return LookupAttachableMember(name);
}

std::string XamlCPP::Core::Proto::XamlType::ToString()
{
    return "XamlType" + name;
}

std::shared_ptr<System::Reflection::Object> XamlCPP::Core::Proto::XamlType::CreateInstance(std::vector<System::Reflection::Variant> const& parameters)
{
    return typeFactory->Create(underlyingType, parameters);
}

XamlType * XamlCPP::Core::Proto::XamlType::Create(
    System::Reflection::Type const * underlyingType,
    Interfaces::ITypeRepository * typeRepository,
    Interfaces::ITypeFactory * typeFactory,
    Interfaces::ITypeFeatureProvider * featureProvider)
{
    System::ThrowIfFailed<System::ArgumentException>(underlyingType, nameof(typeRepository));

    return new XamlType(underlyingType, typeRepository, typeFactory, featureProvider);
}

XamlType * XamlCPP::Core::Proto::XamlType::CreateForBuiltInType(System::Reflection::Type const * type)
{
    System::ThrowIfFailed<System::ArgumentException>(type, nameof(type));

    return new XamlType(type);
}

ReflEx::AttachableMember * XamlCPP::Core::Proto::XamlType::LookupAttachableMember(std::string const & name)
{
    //Note: Attached property currently implementation prefer static methods
    const System::Reflection::IMethod* getter = underlyingType->GetStaticMethod("Get" + name);
    if (!getter || !getter->IsValid())
        getter = &underlyingType->GetRuntimeMethod("Get" + name);
    const System::Reflection::IMethod* setter = underlyingType->GetStaticMethod("Set" + name);
    if (!setter || !setter->IsValid())
        setter = &underlyingType->GetRuntimeMethod("Set" + name);
    return typeRepository->GetAttachableMember(name, getter, setter);
}

std::vector<ReflEx::MemberBase*> XamlCPP::Core::Proto::XamlType::GetAllMembers()
{
    std::vector<ReflEx::MemberBase*> allmembers;
    allmembers.reserve(50);

    underlyingType->VisitInheritClasses([&, this](System::Reflection::Type const* type)
    {
        if (type->GetProperties())
        {
            for (auto iProp = type->GetProperties()->begin(); iProp != type->GetProperties()->end(); ++iProp)
            {
                if (IsValidMember(*iProp))
                {
                    ReflEx::MemberBase* member = GetMember((*iProp)->GetName());
                    if (member)
                        allmembers.push_back(member);
                }
            }
        }

        return true;
    });

    return allmembers;
}

bool XamlCPP::Core::Proto::XamlType::IsValidMember(System::Reflection::meta::Field const * info)
{
    auto hasValidSetter = info->SetMethod() != nullptr && info->SetMethod()->IsPublic();
    auto hasValidGetter = info->GetMethod() != nullptr && info->GetMethod()->IsPublic();
    return hasValidGetter && hasValidSetter;
}

Interfaces::INameScope * XamlCPP::Core::Proto::XamlType::GetNamescope(System::Reflection::Object * instance)
{
    static const System::Reflection::Type& iNameScopeType = typeof(XamlCPP::Interfaces::INameScope);
    if (iNameScopeType.IsAssignableFrom(*instance->GetType()))
        return instance->Dynamic_As<XamlCPP::Interfaces::INameScope>();
    return nullptr;
}

void XamlCPP::Core::Proto::XamlType::BeforeInstanceSetup(System::Reflection::Object * instance)
{
}

void XamlCPP::Core::Proto::XamlType::AfterInstanceSetup(System::Reflection::Object * instance)
{
}

void XamlCPP::Core::Proto::XamlType::AfterAssociationToParent(System::Reflection::Object * instance)
{
}

XamlCPP::Core::Proto::XamlType::XamlType(System::Reflection::Type const * type, Interfaces::ITypeRepository * typeRepository, Interfaces::ITypeFactory * typeTypeFactory, Interfaces::ITypeFeatureProvider * featureProvider)
    : typeRepository(typeRepository)
    , typeFactory(typeTypeFactory)
    , featureProvider(featureProvider)
    , underlyingType(type)
    , name(type->GetName())
{
    System::ThrowIfFailed<System::ArgumentNullException>(type && type->IsValid(), nameof(type));
    System::ThrowIfFailed<System::ArgumentNullException>(typeRepository, nameof(typeRepository));
    System::ThrowIfFailed<System::ArgumentNullException>(featureProvider, nameof(featureProvider));
}

XamlCPP::Core::Proto::XamlType::XamlType(System::Reflection::Type const * type)
    : underlyingType(type)
    , name(type->GetName()) 
{
    System::ThrowIfFailed<System::ArgumentNullException>(type && type->IsValid(), nameof(type));
}

bool XamlCPP::Core::Proto::XamlType::IsCollection() const
{
    static System::Reflection::Type const& icontainerType = typeof(System::Collection::Generic::IContainer);
    bool isContainer = icontainerType.IsAssignableFrom(*underlyingType);
    return isContainer;
}

bool XamlCPP::Core::Proto::XamlType::IsContainer() const
{
    return IsCollection() || IsIndexedCollection();
}

bool XamlCPP::Core::Proto::XamlType::IsIndexedCollection() const
{
    static System::Reflection::Type const& iassociativeContainerType = typeof(System::Collection::Generic::IMapContainer);
    bool isIndexedCollection = iassociativeContainerType.IsAssignableFrom(*underlyingType);
    return isIndexedCollection;
}

bool XamlCPP::Core::Proto::XamlType::NeedsConstructionParameters() const
{
    if (underlyingType->GetConstructors())
    {
        for (auto const& cons : *underlyingType->GetConstructors())
        {
            if (cons->GetMethodInfo().m_argHashStorage.empty())
                return false;
        }

        return true;
    }
    return false;
}
