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

#include "Gestures.h"

#include "UI/Controls/InputElement.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::Interfaces;
using namespace XUI::Core::Observer;
using namespace XUI::UI::Controls;

std::shared_ptr<RoutedEvent> XUI::Core::Input::Gestures::TappedEvent;
std::shared_ptr<RoutedEvent> XUI::Core::Input::Gestures::DoubleTappedEvent;
std::weak_ptr<UI::Controls::Visual> XUI::Core::Input::Gestures::s_lastPress;

void Static_Gestures()
{
    XUI::Core::Input::Gestures::TappedEvent = (RoutedEvent::Register<RoutedEventArgs>(nameof(Tapped), RoutingStrategies(RoutingStrategies::Bubble), &typeof(XUI::Core::Input::Gestures)));
    XUI::Core::Input::Gestures::DoubleTappedEvent = (RoutedEvent::Register<RoutedEventArgs>(nameof(DoubleTapped), RoutingStrategies(RoutingStrategies::Bubble), &typeof(XUI::Core::Input::Gestures)));

    InputElement::PointerPressedEvent.get()->RouteFinished.get_observable().subscribe(XUI::Core::Input::Gestures::PointerPressed);
    InputElement::PointerReleasedEvent.get()->RouteFinished.get_observable().subscribe(XUI::Core::Input::Gestures::PointerReleased);
}

STATIC_POST_INITIALIZER(Static_Gestures)

void XUI::Core::Input::Gestures::PointerPressed(Observer::RoutedEventArgs const & ev)
{
    if (ev.Route == RoutingStrategies::Bubble)
    {
        auto e = (PointerPressedEventArgs const&)ev;

        if (e.ClickCount <= 1)
        {
            s_lastPress = e.Source->shared_from_base_static<Visual>();
        }
        else if (!s_lastPress.expired() && e.ClickCount == 2 && s_lastPress.lock().get() == e.Source)
        {
            RoutedEventArgs event_arg(DoubleTappedEvent.get());
            e.Source->RaiseEvent(event_arg);
        }
    }
}

void XUI::Core::Input::Gestures::PointerReleased(Observer::RoutedEventArgs const & ev)
{
    if (ev.Route == RoutingStrategies::Bubble)
    {
        auto e = (PointerReleasedEventArgs&)ev;

        if (!s_lastPress.expired() && s_lastPress.lock().get() == e.Source)
        {
            RoutedEventArgs event_arg(TappedEvent.get());
            static_cast<InputElement*>(s_lastPress.lock().get())->RaiseEvent(event_arg);
        }
    }
}
