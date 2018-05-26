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

#include "XamlCPP.h"

#include "XmlLoader.h"
#include "Core/Default/RuntimeTypeSource.h"
#include "Core/Default/DefaultParserFactory.h"
#include "Reflection/Runtime/ReflectionModule.h"

using namespace XamlCPP;

std::shared_ptr<Interfaces::ILoader>            Xaml::xmlLoader = nullptr;
std::shared_ptr<Interfaces::IRuntimeTypeSource> Xaml::runtimeTypeSource = nullptr;
System::ServiceContainer*                       Xaml::UIServiceContext = nullptr;

std::shared_ptr<Interfaces::ILoader> Xaml::DefaultLoader(System::ServiceContainer* serviceContext)
{
    if (serviceContext)
        Xaml::UIServiceContext = serviceContext;

    System::ThrowIfFailed<System::InvalidOperationException>(Xaml::UIServiceContext);

    if (!xmlLoader)
    {
        if (!runtimeTypeSource)
            runtimeTypeSource = Core::RuntimeTypeSource::FromAttributes(System::Reflection::meta::ReflectionModulManager::Instance().GetAssemblies());
        Interfaces::IParserFactory* factory = new Core::DefaultParserFactory(runtimeTypeSource);
        auto sharedparser = std::make_shared<::XamlCPP::XmlLoader>(factory);
        xmlLoader = sharedparser->shared_from_base_static<Interfaces::ILoader>();
    }

    return xmlLoader;
}

std::shared_ptr<Interfaces::ILoader> XamlCPP::Xaml::CreateLoaderContext()
{
    if (!runtimeTypeSource)
        runtimeTypeSource = Core::RuntimeTypeSource::FromAttributes(System::Reflection::meta::ReflectionModulManager::Instance().GetAssemblies());
    Interfaces::IParserFactory* factory = new Core::DefaultParserFactory(runtimeTypeSource);
    auto sharedparser = std::make_shared<::XamlCPP::XmlLoader>(factory);
    xmlLoader = sharedparser->shared_from_base_static<Interfaces::ILoader>();

    return xmlLoader;
}

bool XamlCPP::Xaml::IsUnsetValue(System::Reflection::Variant const & v)
{
    static const System::Reflection::Type* UnSetType = &System::Reflection::ReflectionModulManager::Instance().GetType("XUI::Core::Dependency::Unset");

    System::ThrowIfFailed<System::AggregateException>(UnSetType->IsValid(), "Required XUI framework");

    return v.IsObject() && v.ToObject()->Is(*UnSetType);
}
