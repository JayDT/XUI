/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2014 Steven Kirk <The Avalonia Project>
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
#include "Setter.h"
#include "Core/Dependencies/DependencyPropertyMgr.h"
#include "UI/Controls/Control.h"
   
#include "XamlCPP/XamlCPP.h"
#include "XamlCPP/DataBind/InstancedBinding.h"
#include "XamlCPP/Interfaces/IStyle.h"
#include "XamlCPP/Interfaces/ITemplate.h"
#include "XamlCPP/Core/ReflEx/TypeConversion/CommonValueConversion.h"

using namespace XUI::Styling;

rxcpp::subscription Setter::Apply(XamlCPP::Interfaces::IStyle *style, XamlCPP::Interfaces::IStyleable *control, std::shared_ptr<rxcpp::subjects::subject<bool>> const& activator)
{
    static const System::Reflection::Type* ITemplateType = &typeof(XamlCPP::Interfaces::ITemplate);

    System::ThrowIfFailed<System::ArgumentException>(control != nullptr);
    System::ThrowIfFailed<System::AggregateException>(ITemplateType->IsValid());

	auto* target_control = static_cast<UI::Controls::Control*>(control);
    if (!TargetName.empty())
    {
        if (!target_control->NamedScope)
            return rxcpp::subscription();

        auto namedControl = target_control->NamedScope->Find(TargetName);
        if (!namedControl && target_control->Template)
            namedControl = target_control->Template->FindName(TargetName, control->ToIDependencyObject()->As<UI::Controls::Control>());
            
        if (!namedControl)
            return rxcpp::subscription();

        target_control = namedControl->Static_As<UI::Controls::Control>();
    }

    //auto description = style == nullptr ? "" : style->ToString();
    
    if (Property == nullptr)
        throw System::InvalidOperationException("Setter.Property must be set.");

    System::Reflection::Variant value = _value;
    auto binding = _value.IsObject() ? _value.ToObject()->As<XamlCPP::Interfaces::IBinding>() : nullptr;

    if (binding == nullptr)
    {
        auto _template = _value.IsObject() ? _value.ToObject()->As<XamlCPP::Interfaces::ITemplate>() : nullptr;
        bool isPropertyOfTypeITemplate = ITemplateType->IsAssignableFrom(*Property->PropertyType);
    
        if (_template != nullptr && !isPropertyOfTypeITemplate)
        {
            auto materialized = _template->Build(nullptr);
            value = materialized;
        }
    
        if (activator == nullptr)
        {
            if (value.GetType() != Core::Dependency::DependencyPropertyMgr::UnsetValue.GetType())
                System::ThrowIfFailed<System::InvalidOperationException>((Property->GetPropertyType()->IsEnum() && value.GetType()->IsPrimitive()) || 
                    *Property->GetPropertyType() == typeid(System::Reflection::Variant) || 
                    Property->GetPropertyType()->IsAssignableFrom(*value.GetType()), "Invalid type conversion");

            XamlCPP::BindingLevel lvl = style == nullptr ? XamlCPP::BindingLevel::StyleTrigger : XamlCPP::BindingLevel::Style;

            if (lvl == XamlCPP::BindingLevel::StyleTrigger)
            {
                auto ret = target_control->ToIDependencyObject()->Bind(Property, &_setterSubject.get_observable(), lvl);
                _setterSubject.get_observer().on_next(value); //get_subscriber().on_next(value);
                return ret;
            }

            target_control->SetValue(Property, value, XamlCPP::BindingLevel::Style);

            return rxcpp::make_subscription([this, control = target_control->weak_from_base_static<UI::Controls::Control>()]()
            {
                if (!control.expired())
                    control.lock()->SetValue(Property, Core::Dependency::DependencyPropertyMgr::UnsetValue, XamlCPP::BindingLevel::Style);
            });
        }

        if (!_setterObserver)
        {
            _setterObserver = std::make_shared<XamlCPP::Core::Dependency::GenericObservable>(_setterSubject.get_observable().map([this, _activator = activator](System::Reflection::Variant const& value)
            {
                bool active = false;
                _activator->get_subscriber().on_next(active);
                return active ? Value : Core::Dependency::DependencyPropertyMgr::UnsetValue;
            }));
        }
        auto ret = target_control->ToIDependencyObject()->Bind(Property, _setterObserver.get(), style == nullptr ? XamlCPP::BindingLevel::StyleTrigger : XamlCPP::BindingLevel::Style);
        _setterSubject.get_observer().on_next(value); //.get_subscriber().on_next(value);
        return ret;
    }

	binding->SetBindingLevel(style == nullptr ? XamlCPP::BindingLevel::StyleTrigger : XamlCPP::BindingLevel::Style);
	return Core::Dependency::DependencyObject::Bind(target_control, static_cast<Core::Dependency::DependencyProperty*>(Property), binding);
}

std::shared_ptr<XamlCPP::DataBind::InstancedBinding> Setter::SetterClone(XamlCPP::DataBind::InstancedBinding *sourceInstance, XamlCPP::Interfaces::IStyle *style, std::shared_ptr<rxcpp::subjects::subject<bool>> const& activator)
{
    std::shared_ptr<XamlCPP::DataBind::InstancedBinding> cloned;
    
    return cloned;
}