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
#include "DynamicResourceBinding.h"
#include "Core/Dependencies/IDependencyObject.h"
#include "Runtime/ReflectionModule.h"
#include "DataBind/InstancedBinding.h"
#include "Core/TypeExtensions/TypeConverter.h"
#include "XamlCPP.h"
#include "Interfaces/IStyle.h"
#include "Interfaces/IStyleHost.h"
#include "Interfaces/INamedScope.h"
#include "ValueContext.h"

using namespace XamlCPP;

std::shared_ptr<DataBind::InstancedBinding> XamlCPP::DataBind::DynamicResourceBinding::Initiate(Core::Dependency::IDependencyObject * target, Core::Dependency::IPropertyObject * targetProperty, System::Reflection::Object * anchor, bool enableDataValidation, std::shared_ptr<XamlCPP::Interfaces::INameScope>* namedScope)
{
    auto control = target ? target->Dynamic_As<Interfaces::IResourceNode>() : _anchor;
    if (!control)
        control = anchor ? anchor->Dynamic_As<Interfaces::IResourceNode>() : nullptr;

    if (control != nullptr)
    {
		if (m_markup)
		{
			Core::ReflEx::ValueContext valueContext(Xaml::GetRuntimeTypeSource(), nullptr);
			Core::MarkupExtensionContext context;
			context.setTargetObject(target);
			context.setTargetProperty(nullptr);
			context.setValueContext(&valueContext);
			System::Reflection::Variant value = m_markup->ProvideValue(&context);
			System::Reflection::meta::Variant newValue = value.GetType() == targetProperty->PropertyType ? value : Core::TypeExtensions::DefaultValueConverter::Instance.Convert(value, targetProperty->PropertyType, System::Reflection::Variant::Empty, 0);

			target->SetValue(targetProperty, value, _bindinglevel);
		}
		else if (!m_ResourceKey.empty())
		{
			auto observer = control->ToIDependencyObject2()->GetObservable(
				// Add Event
				[resourceKey = m_ResourceKey, owner = control](rxcpp::subscriber<System::Reflection::meta::Variant> const& observer) -> System::Events::IEventHook*
				{
					System::Reflection::Variant const& value = owner->FindResource(resourceKey);
					observer.on_next(value);

					return owner->ResourcesChanged += [lowner = owner, lresourceKey = resourceKey, lobserver = observer](void* sender, Interfaces::ResourceChangedEventArgs const& e)
					{
						if (lresourceKey == e.ResourceKey)
						{
							System::Reflection::Variant const& value = lowner->FindResource(lresourceKey);
							if (value.GetBase())
								lobserver.on_next(value);
						}
					};
				},
				// Remove Event
				[owner = control](System::Events::IEventHook* event)
				{
					owner->ResourcesChanged -= event;
				}
			);
			auto instancedBinding = std::make_shared<DataBind::InstancedBinding>(observer, BindingMode::OneWay, _bindinglevel);

			instancedBinding->Converter = &Core::TypeExtensions::DefaultValueConverter::Instance;
			//instancedBinding->ConverterParameter = ConverterParameter;
			instancedBinding->TargetProperty = targetProperty;

			return instancedBinding;
		}
    }

    return nullptr;
}
