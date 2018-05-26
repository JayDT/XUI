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

#include "Style.h"
#include "UI/Controls/Control.h"
#include "Core/Dependencies/DependencyPropertyMgr.h"
#include "Core/Dependencies/DependencyObject.h"

using namespace XUI::Styling;

void XUI::Styling::Style::Attach(XamlCPP::Interfaces::IStyleable * control, XamlCPP::Interfaces::IStyleHost * container)
{
    //TODO: triggers
    if (!TargetType || TargetType->IsAssignableFrom(*control->ToIDependencyObject()->GetType()))
    {
        if (BasedOn)
            BasedOn->Attach(control, container);

        auto& subs = GetSubscriptions(control);

        for (auto& setter : Setters.GetContainer())
        {
            auto sub = setter->Apply(this, control, nullptr);
            if (_applied.find(control) != _applied.end())
                subs.push_back(sub);
            else
                return;
        }

        for (auto& triggers : Triggers.GetContainer())
        {
            auto sub = triggers->Apply(static_cast<XUI::UI::Controls::Control*>(control));
            if (_applied.find(control) != _applied.end())
                subs.push_back(sub);
            else
                return;
        }
    }
}

void XUI::Styling::Style::Detach(XamlCPP::Interfaces::IStyleable * control)
{
    if (BasedOn)
        BasedOn->Detach(control);

    auto iSubscriptions = _applied.find(control);

    if (iSubscriptions != _applied.end())
    {
        for (auto& subscription : iSubscriptions->second)
        {
            if (subscription.is_subscribed())
                subscription.unsubscribe();
        }

        _applied.erase(iSubscriptions);
    }
}

System::Reflection::Variant const& XUI::Styling::Style::FindResource(const std::string & name) const
{
    auto _resources_iterator = _resources.GetNativeEnumerator().find(name);
    if (_resources_iterator != _resources.GetNativeEnumerator().end())
    {
        return _resources_iterator->second;
    }

    return Core::Dependency::DependencyPropertyMgr::UnsetValue;
}

std::vector<rxcpp::subscription>& XUI::Styling::Style::GetSubscriptions(XamlCPP::Interfaces::IStyleable * control)
{
    auto _applied_iterator = _applied.find(control);
    if (_applied_iterator == _applied.end())
    {
        auto& subscriptions = _applied[control];
        if (subscriptions.capacity() < 16)
            subscriptions.reserve(16);
        subscriptions.push_back(control->GetStyleDetach()->get_observable().subscribe([weakStyle = weak_from_base_static<XUI::Styling::Style>()](XamlCPP::Interfaces::IStyleable * control)
        {
            if (!weakStyle.expired())
                weakStyle.lock()->ControlDetach(control);
        }));
        _applied_iterator = _applied.emplace(control, subscriptions).first;
    }

    return  _applied_iterator->second;
}

/// <summary>
/// Called when a control's <see cref="IStyleable.StyleDetach"/> is signalled to remove
/// all applied styles.
/// </summary>
/// <param name="control">The control.</param>

void XUI::Styling::Style::ControlDetach(XamlCPP::Interfaces::IStyleable * control)
{
    Detach(control);
}
