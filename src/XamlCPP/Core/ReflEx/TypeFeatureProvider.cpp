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
#include "TypeFeatureProvider.h"
#include "Interfaces/ReflAttributes.h"
#include "Core/ReflEx/TypeConverterRegistration.h"
#include "Core/ReflEx/DependencyRegistration.h"
#include "Core/Metadata.h"

#include <Runtime/Type.h>

#include <cpplinq/linq.hpp>

XamlCPP::Core::ReflEx::TypeFeatureProvider::TypeFeatureProvider(std::shared_ptr<Interfaces::ITypeConverterProvider> const& converterProvider)
    : privateConverterProvider(converterProvider)
{
}

XamlCPP::Interfaces::ITypeConverterProvider * XamlCPP::Core::ReflEx::TypeFeatureProvider::getConverterProvider() const
{
    return privateConverterProvider.get();
}

XamlCPP::Interfaces::ITypeConverter * XamlCPP::Core::ReflEx::TypeFeatureProvider::GetTypeConverter(std::size_t typehash)
{
    return privateConverterProvider->GetTypeConverter(typehash);
}

void XamlCPP::Core::ReflEx::TypeFeatureProvider::AddTypeConverter(TypeConverterRegistration const& typeConverterRegistration)
{
    privateConverterProvider->Add(typeConverterRegistration);
}

std::string XamlCPP::Core::ReflEx::TypeFeatureProvider::GetContentPropertyName(const System::Reflection::Type * type)
{
    return GetMetadata(type).get()->ContentProperty;
}

std::vector<XamlCPP::Core::TypeConverterRegistration> XamlCPP::Core::ReflEx::TypeFeatureProvider::getTypeConverters()
{
    return privateConverterProvider->getTypeConverters();
}

std::shared_ptr<XamlCPP::Core::Metadata> XamlCPP::Core::ReflEx::TypeFeatureProvider::GetMetadata(const System::Reflection::Type * type)
{
    auto metadata = metadatas.Get(type);

    if (!metadata)
    {
        type->VisitDescendInheritClasses([&](const System::Reflection::Type* _type)
        {
            metadata = metadatas.Get(_type);
            return metadata ? false : true;
        });
    }

    return metadata;
}

void XamlCPP::Core::ReflEx::TypeFeatureProvider::RegisterMetadata(const System::Reflection::Type * type, Metadata * metadata)
{
    metadatas.Register(type, metadata);
}

std::string GetContentProperty(const System::Reflection::Type *type)
{
    std::string result;

    type->VisitInheritClasses([&](const System::Reflection::Type *type)
    {
        //if (!result.empty())
        //    return true;

        if (type->GetProperties())
        {

            for (System::Reflection::Field* x : *type->GetProperties())
            {
                const XamlCPP::ContentAttribute* content = x->GetMeta().GetCustomAttribute<XamlCPP::ContentAttribute>();
                if (content)
                {
                    if (!result.empty())
                        throw System::NotSupportedException("Content property defined more than once on.");

                    result = x->GetName();
                }
            }
        }

        return result.empty();
    });

    return result;
}

std::string GetRuntimePropertyName(const System::Reflection::Type *type)
{
    std::string result;

    type->VisitInheritClasses([&](const System::Reflection::Type *type)
    {
        //if (!result.empty())
        //    return true;

        if (type->GetProperties())
        {
            auto properties = cpplinq::from(*type->GetProperties()).where([&](System::Reflection::meta::Field *x)
            {
                return x->GetMeta().GetCustomAttribute<XamlCPP::NameAttribute>() != nullptr;
            });

            for (auto property_ : properties)
            {
                if (!result.empty())
                    throw System::NotSupportedException("Content property defined more than once on.");

                result = property_->GetName();
            }
        }

        if (result.empty())
        {
            auto const& nameProp = type->GetProperty("Name");
            if (nameProp.IsValid())
                result = "Name";
        }

        return result.empty();
    });

    return result;
}

std::list<XamlCPP::Core::ReflEx::DependencyRegistration> GetDependencies(const System::Reflection::Type * type)
{
    std::list<XamlCPP::Core::ReflEx::DependencyRegistration> result;

    type->VisitInheritClasses([&](const System::Reflection::Type *type)
    {
        //if (!result.empty())
        //    return true;

        if (type->GetProperties())
        {
            for (auto x : *type->GetProperties())
            {
                XamlCPP::DependsOnAttribute const* attr = x->GetMeta().GetCustomAttribute<XamlCPP::DependsOnAttribute>();
                if (attr)
                    result.push_back(XamlCPP::Core::ReflEx::DependencyRegistration(x->GetName(), attr->getPropertyName()));
            }
        }

        return result.empty();
    });

    return result;
}

void XamlCPP::Core::ReflEx::TypeFeatureProvider::FillFromAttributes(std::set<System::Reflection::Type const*> &allExportedTypes)
{
    for (auto t : allExportedTypes)
    {
        auto dependencies = GetDependencies(t);
        auto contentProperty = GetContentProperty(t);
        auto runtimePropertyName = GetRuntimePropertyName(t);

        auto metadata = std::make_shared<Metadata>();
        metadata->RuntimePropertyName = runtimePropertyName;
        metadata->PropertyDependencies = dependencies;
        metadata->ContentProperty = contentProperty;

        RegisterMetadata(t, metadata.get());
    }
}
