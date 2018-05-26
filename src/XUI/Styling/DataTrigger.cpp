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
#include "DataTrigger.h"
#include "UI/Controls/Control.h"

using namespace XUI;
using namespace XUI::Styling;

rxcpp::subscription DataTrigger::Apply(XUI::UI::Controls::UIElement * control)
{
    if (Binding)
    {
        //auto& bindElement = _applied[control];
        //bindElement._triggerObserver = control->GetObservable(Property);
        //
        //UI::Controls::Control* targetControl = control->As<UI::Controls::Control>();
        //
        //bindElement._triggerObserver->subscribe([this, &bindElement, element = control->weak_from_base_static<XUI::UI::Controls::Control>(), setterTarget = targetControl->weak_from_base_static<XUI::UI::Controls::Control>()](System::Reflection::Variant const& value)
        //{
        //    if (Value.GetType() != value.GetType())
        //        return;
        //
        //    auto control = element.lock();
        //    auto setterControl = setterTarget.lock();
        //    if (Value.IsFastEqual(value))
        //    {
        //        for (auto const& setter : Setters.GetContainer())
        //        {
        //            auto sub = setter->Apply(nullptr, setterControl.get(), nullptr);
        //            bindElement._triggerSetterDetach.push_back(sub);
        //        }
        //
        //        for (auto& action : _enterActions)
        //        {
        //            action->Invoke(control.get());
        //        }
        //
        //        bindElement._isEntered = true;
        //    }
        //    else if (bindElement._isEntered)
        //    {
        //        bindElement._isEntered = false;
        //
        //        for (auto& sub : bindElement._triggerSetterDetach)
        //        {
        //            if (sub.is_subscribed())
        //                sub.unsubscribe();
        //        }
        //
        //        bindElement._triggerSetterDetach.clear();
        //
        //        for (auto& action : _exitActions)
        //        {
        //            action->Invoke(control.get());
        //        }
        //    }
        //});
        //
        //rxcpp::subscription unbindEvent = control->Bind(Property, bindElement._triggerObserver.get(), Core::Dependency::BindingLevel::StyleTrigger);
        //
        //return rxcpp::make_subscription([this, element = control, unbind = unbindEvent]()
        //{
        //    if (unbind.is_subscribed())
        //        unbind.unsubscribe();
        //
        //    _applied.erase(element);
        //});
    }
    return rxcpp::subscription();
}
