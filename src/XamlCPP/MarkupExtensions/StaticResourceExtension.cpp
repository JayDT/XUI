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
#include "StaticResourceExtension.h"
#include "StoredInstance.h"
#include "Proto/XamlType.h"
#include "Core/Dependencies/IDependencyObject.h"
#include "DataBind/DelayedBinding.h"
#include "XamlCPP.h"

System::Reflection::Variant XamlCPP::Markup::StaticResourceExtension::ProvideValue(Core::MarkupExtensionContext * markupExtensionContext)
{
    static const System::Reflection::Type& idepType = typeof(Core::Dependency::IDependencyObject);

	if (GetResourceKey().empty() && m_markup)
		return m_markup->ProvideValue(markupExtensionContext);

    auto control = markupExtensionContext->getTargetObject().IsObject() ? markupExtensionContext->getTargetObject().ToObject()->As<Core::Dependency::IDependencyObject>() : nullptr;
    if (control)
    {
        auto resource = control->FindResource(GetResourceKey());
        if (resource.GetBase())
            return resource;
    }

    auto instances = markupExtensionContext->getValueContext()->getTopDownValueContext()->GetStoredInstances();
    for (auto const& si : instances)
    {
        if (!idepType.IsAssignableFrom(*si.GetXamlType()->UnderlyingType()))
            continue;

        auto instance = si.GetInstance();
        control = instance->As<Core::Dependency::IDependencyObject>();
        if (control)
        {
            auto resource = control->FindResource(GetResourceKey());
            if (resource.GetBase())
                return resource;
        }
    }

    if (control && markupExtensionContext->getTargetProperty())
    {
	    auto* propertyService = Xaml::UIServiceContext->Get<XamlCPP::Core::Dependency::IDependencyPropertyMgr>();
        if (propertyService)
        {
            Core::Dependency::IPropertyObject * property = propertyService->FindRegistered(control->GetType(), markupExtensionContext->getTargetProperty()->GetName());
            if (property)
            {
                //VisualParentProperty
                Markup::DelayedBinding::Add(control->shared_from_this(), property, [&](Core::Dependency::IDependencyObject * control)
                {
                    return GetValue(control);
                });
                return System::Reflection::Variant::Empty;
            }
        }
    }
    
    throw System::KeyNotFoundException(System::String::format("Static resource '{0}' not found.", GetResourceKey()).c_str());
}

System::Reflection::Variant XamlCPP::Markup::StaticResourceExtension::GetValue(Core::Dependency::IDependencyObject * control)
{
    return control->FindResource(GetResourceKey());
}
