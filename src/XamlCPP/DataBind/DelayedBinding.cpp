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
#include "DelayedBinding.h"
#include "DataBind/Binding.h"

using namespace XamlCPP;

std::unordered_map<Core::Dependency::IDependencyObject*, std::vector<Markup::DelayedBinding::Entry*>> XamlCPP::Markup::DelayedBinding::_entries;

void XamlCPP::Markup::DelayedBinding::Add(std::shared_ptr<Core::Dependency::IDependencyObject> const & target, Core::Dependency::IPropertyObject * _property, XamlCPP::Interfaces::IBinding * binding)
{
    if (target->IsInited())
    {
        target->Bind(_property, binding);
    }
    else
    {
        System::ThrowIfFailed<System::NotSupportedException>(target->OnInitialized());
        auto& vec = _entries[target.get()];
        if (vec.capacity() < 16)
            vec.reserve(16);
        vec.emplace_back(new BindingEntry(binding, _property));
        *target->OnInitialized() += System::make_eventhandler(&DelayedBinding::EventApplyBindings);
    }
}

void XamlCPP::Markup::DelayedBinding::Add(std::shared_ptr<Core::Dependency::IDependencyObject> const & target, Core::Dependency::IPropertyObject * _property, std::function<System::Reflection::Variant(Core::Dependency::IDependencyObject*)> const & value)
{
    if (target->IsInited())
    {
        auto _value = value(target.get());
        _property->SetValue(target.get(), _value);
    }
    else
    {
        System::ThrowIfFailed<System::NotSupportedException>(target->OnInitialized());
        auto& vec = _entries[target.get()];
        if (vec.capacity() < 16)
            vec.reserve(16);
        vec.emplace_back(new PropertyValueEntry(value, _property));
        *target->OnInitialized() += System::make_eventhandler(&DelayedBinding::EventApplyBindings);
    }
}

/// <summary>
/// Applies any delayed bindings to a control.
/// </summary>
/// <param name="control">The control.</param>

void XamlCPP::Markup::DelayedBinding::ApplyBindings(Core::Dependency::IDependencyObject * control)
{
    auto iEntry = _entries.find(control);
    if (iEntry != _entries.end())
    {
        for (auto const& binding : iEntry->second)
        {
            binding->Apply(control);
            delete binding;
        }

        _entries.erase(iEntry);
    }
}

void XamlCPP::Markup::DelayedBinding::EventApplyBindings(void * sender, System::Events::EventArg & e)
{
    auto target = static_cast<System::Reflection::Object*>(sender)->Dynamic_As<Core::Dependency::IDependencyObject>();
    ApplyBindings(target);
    if (target->OnInitialized())
        *target->OnInitialized() -= System::make_eventhandler(&DelayedBinding::EventApplyBindings);
}

XamlCPP::Markup::DelayedBinding::BindingEntry::BindingEntry(XamlCPP::Interfaces::IBinding * binding, Core::Dependency::IPropertyObject * _property)
{
    Binding = binding->shared_from_this();
    Property = _property;
}

void XamlCPP::Markup::DelayedBinding::BindingEntry::Apply(Core::Dependency::IDependencyObject * control)
{
    control->Bind(Property, Binding.get());
}

void XamlCPP::Markup::DelayedBinding::PropertyValueEntry::Apply(Core::Dependency::IDependencyObject * control)
{
    auto value = Value(control);
    Property->SetValue(control, value);
}
