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
#include "Trigger.h"
#include "UI/Controls/Control.h"

using namespace XUI;
using namespace XUI::Styling;

rxcpp::subscription Trigger::Apply(XUI::UI::Controls::UIElement * control)
{
    if (Property)
    {
        auto& bindElement = _applied[control];

        UI::Controls::Control* targetControl = control->As<UI::Controls::Control>();
        if (targetControl && !SourceName.empty())
        {
            if (!targetControl->NamedScope)
                return rxcpp::subscription();

            auto namedControl = targetControl->NamedScope->Find(SourceName);
            if (!namedControl && targetControl->Template)
                namedControl = targetControl->Template->FindName(SourceName, control->As<UI::Controls::Control>());

            if (!namedControl)
                return rxcpp::subscription();

            targetControl = namedControl->Dynamic_As<UI::Controls::Control>();
        }

        bindElement._triggerObserver = targetControl->GetObservable(Property);

        rxcpp::subscription unbindEvent(std::move(bindElement._triggerObserver->subscribe([this, &bindElement, element = targetControl->weak_from_base_static<XUI::UI::Controls::Control>(), setterTarget = targetControl->weak_from_base_static<XUI::UI::Controls::Control>()](System::Reflection::Variant const& value)
        {
            auto TriggerValueType = Value.GetType();
            if (TriggerValueType != value.GetType())
            {
                if (!value.GetType()->IsEnum() || !TriggerValueType->IsPrimitive())
                    return;
            }

            auto control = element.lock();
            auto setterControl = setterTarget.lock();
            if (Value.IsFastEqual(value))
            {
                for (auto const& setter : Setters.GetContainer())
                {
                    auto sub = setter->Apply(nullptr, setterControl.get(), nullptr);
                    bindElement._triggerSetterDetach.push_back(sub);
                }

                for (auto& action : _enterActions)
                {
                    action->Invoke(control.get());
                }

                bindElement._isEntered = true;
            }
            else if (bindElement._isEntered)
            {
                bindElement._isEntered = false;

                for (auto& sub : bindElement._triggerSetterDetach)
                {
                    if (sub.is_subscribed())
                        sub.unsubscribe();
                }

                bindElement._triggerSetterDetach.clear();

                for (auto& action : _exitActions)
                {
                    action->Invoke(control.get());
                }
            }
        })));

        return rxcpp::make_subscription([this, element = targetControl, unbind = unbindEvent]()
        {
            if (unbind.is_subscribed())
                unbind.unsubscribe();

            _applied.erase(element);
        });
    }
    return rxcpp::subscription();
}
