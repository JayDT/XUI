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
#include "MultiBinding.h"
#include "InstancedBinding.h"
#include "Core/Dependencies/IDependencyObject.h"
#include "XamlCPP.h"
#include "Interfaces/INamedScope.h"

#include <cpplinq/linq.hpp>

using namespace XamlCPP;

std::shared_ptr<DataBind::InstancedBinding> XamlCPP::Markup::MultiBinding::Initiate(Core::Dependency::IDependencyObject * target, Core::Dependency::IPropertyObject * targetProperty, System::Reflection::Object * anchor, bool enableDataValidation, std::shared_ptr<XamlCPP::Interfaces::INameScope>* namedScope)
{
    System::ThrowIfFailed<System::NotSupportedException>(Converter, "MultiBinding without Converter not currently supported.");
    
    auto targetType = targetProperty == nullptr ? nullptr : (targetProperty->PropertyType != nullptr && targetProperty->PropertyType->IsValid()) ? targetProperty->PropertyType : &typeof(System::Reflection::Object);
    auto result = std::make_shared<Core::Dependency::GenericObservable>(); //BehaviorSubject<void*>(AvaloniaProperty::UnsetValue);
    auto children = cpplinq::from(Bindings).select([&](std::shared_ptr<Interfaces::IBinding>& x)
    {
        return x->Initiate(target, nullptr);
    }).to_vector();

    for (std::shared_ptr<DataBind::InstancedBinding>& x : children)
    {
        if (x->Subject)
        {
            result->subscribe(std::move(x->Subject->get_observer().get_subscriber()));
        }
    }

    auto instancedBinding = std::make_shared<DataBind::InstancedBinding>(result, Mode, BindingLevel);

    auto bindingService = Xaml::UIServiceContext->Get<XamlCPP::Interfaces::IInstancedBindingManager>();
    if (bindingService)
        bindingService->AddInstancedBinding(instancedBinding.get());

    return instancedBinding;
}

/// <summary>
/// Applies a binding subject to a property on an instance.
/// </summary>
/// <param name="target">The target instance.</param>
/// <param name="property">The target property.</param>
/// <param name="subject">The binding subject.</param>

void XamlCPP::Markup::MultiBinding::Bind(Core::Dependency::IDependencyObject * target, Core::Dependency::IPropertyObject * _property, Core::Dependency::BindingSubject * subject)
{
    auto mode = Mode == BindingMode::Default ? _property->GetDefaultBindingMode(target->GetType()) : Mode;
    
    switch (mode)
    {
        case BindingMode::Default:
        case BindingMode::OneWay:
            target->Bind(_property, &subject->Subject.get_observable(), BindingLevel);
            break;
        case BindingMode::TwoWay:
            throw System::NotSupportedException("TwoWay MultiBinding not currently supported.");
        case BindingMode::OneTime:
            throw System::NotSupportedException("TwoWay MultiBinding not currently supported.");
            //target->GetObservable(Control->DataContextProperty).Subscribe([&] (void *dataContext)
            //{
            //    subject->Take(1)->Subscribe([&] (void *x)
            //    {
            //        target->SetValue(_property, x, Priority);
            //    });
            //});
            break;
        case BindingMode::OneWayToSource:
            throw System::NotSupportedException("TwoWay MultiBinding not currently supported.");
            //target->GetObservable(_property).Subscribe(subject);
            break;
    }
}

System::Reflection::Variant Markup::MultiBinding::ConvertValue(std::vector<System::Reflection::Variant>& values,
                                                               const System::Reflection::Type* targetType)
{
	auto converted = Converter->Convert(values, targetType, nullptr, 0);

	if (!converted.GetBase() && FallbackValue.GetBase())
	{
		converted = FallbackValue;
	}

	return converted;
}
