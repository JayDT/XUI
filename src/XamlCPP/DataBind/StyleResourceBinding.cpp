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
#include "StyleResourceBinding.h"
#include "Core/Dependencies/IDependencyObject.h"
#include "Runtime/ReflectionModule.h"
#include "DataBind/InstancedBinding.h"
#include "XamlCPP.h"
#include "Interfaces/IStyle.h"
#include "Interfaces/IStyleHost.h"
#include "Interfaces/INamedScope.h"

#undef FindResource

using namespace XamlCPP;

std::shared_ptr<DataBind::InstancedBinding> XamlCPP::Markup::StyleResourceBinding::Initiate(Core::Dependency::IDependencyObject * target, Core::Dependency::IPropertyObject * targetProperty, System::Reflection::Object * anchor, bool enableDataValidation, std::shared_ptr<XamlCPP::Interfaces::INameScope>* namedScope)
{
    static const System::Reflection::Type* ControlType = &System::Reflection::ReflectionModulManager::Instance().GetType("XUI::UI::Controls::Control");

    System::ThrowIfFailed<System::AggregateException>(ControlType->IsValid(), "Required XUI framework");

    auto host = target ? target->As<Interfaces::IStyleHost>() : nullptr;
    if (!host && anchor)
        host = anchor->As<Interfaces::IStyleHost>();
    auto style = anchor ? anchor->As<Interfaces::IStyle>() : nullptr;
    auto resource = System::Reflection::Variant();

    if (host != nullptr)
    {
        resource = host->FindStyleResource(Name);
    }
    else if (style != nullptr)
    {
        resource = style->FindResource(Name);
    }

    if (resource.GetBase())
    {
        auto instancedBinding = std::make_shared<DataBind::InstancedBinding>(resource, BindingLevel);

        auto bindingService = Xaml::UIServiceContext->Get<XamlCPP::Interfaces::IInstancedBindingManager>();
        if (bindingService)
            bindingService->AddInstancedBinding(instancedBinding.get());

        return instancedBinding;
    }
    return nullptr;
}
