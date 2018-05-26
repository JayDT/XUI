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

#include "Thumb.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI::UI;

STATIC_CLASS_PRE_INITIALIZER(Thumb, Thumb::StaticClassInitializer)

std::shared_ptr<XUI::Core::Observer::RoutedEvent> Thumb::DragStartedEvent;
std::shared_ptr<XUI::Core::Observer::RoutedEvent> Thumb::DragDeltaEvent;
std::shared_ptr<XUI::Core::Observer::RoutedEvent> Thumb::DragCompletedEvent;

void Thumb::StaticClassInitializer()
{
    DragStartedEvent    = XUI::Core::Observer::RoutedEvent::Register<Thumb, Interfaces::VectorEventArgs>("DragStarted",     XUI::Core::Observer::RoutingStrategies::Bubble);
    DragDeltaEvent      = XUI::Core::Observer::RoutedEvent::Register<Thumb, Interfaces::VectorEventArgs>("DragDelta",       XUI::Core::Observer::RoutingStrategies::Bubble);
    DragCompletedEvent  = XUI::Core::Observer::RoutedEvent::Register<Thumb, Interfaces::VectorEventArgs>("DragCompleted",   XUI::Core::Observer::RoutingStrategies::Bubble);

    DragStartedEvent->AddClassSpecHandler<Interfaces::VectorEventArgs, Thumb>(System::make_eventhandler([](void* x, Interfaces::VectorEventArgs& e)
    {
        static_cast<Thumb*>((System::Reflection::Object*)x)->OnDragStarted(e);
    }));
    DragDeltaEvent->AddClassSpecHandler<Interfaces::VectorEventArgs, Thumb>(System::make_eventhandler([](void* x, Interfaces::VectorEventArgs& e)
    {
        static_cast<Thumb*>((System::Reflection::Object*)x)->OnDragDelta(e);
    }));
    DragCompletedEvent->AddClassSpecHandler<Interfaces::VectorEventArgs, Thumb>(System::make_eventhandler([](void* x, Interfaces::VectorEventArgs& e)
    {
        static_cast<Thumb*>((System::Reflection::Object*)x)->OnDragCompleted(e);
    }));
}

XUI::UI::Thumb::Thumb()
    : DragStarted(this)
    , DragDelta(this)
    , DragCompleted(this)
{
}

XUI::UI::Thumb::~Thumb()
{
}

void Thumb::OnDragStarted(XUI::Interfaces::VectorEventArgs& e)
{
}

void Thumb::OnDragDelta(XUI::Interfaces::VectorEventArgs& e)
{
}

void Thumb::OnDragCompleted(XUI::Interfaces::VectorEventArgs& e)
{
}

void Thumb::OnPointerMoved(XUI::Interfaces::PointerEventArgs& e)
{
    if (_dragStarted)
    {
        Interfaces::VectorEventArgs ev;
        ev.RoutedEvent = DragDeltaEvent.get();
        ev.Vector = e.GetPosition(this) - _lastPoint;

        RaiseEvent(ev);

        e.Handled = true; // wpf never report for parents
    }
}

void Thumb::OnPointerPressed(XUI::Interfaces::PointerPressedEventArgs& e)
{
    e.Device->Capture(this);
    _lastPoint = e.GetPosition(this);

    Interfaces::VectorEventArgs ev;
    ev.RoutedEvent = DragStartedEvent.get();
    ev.Vector = _lastPoint;
    _dragStarted = true;

    RaiseEvent(ev);

    e.Handled = true; // wpf never report for parents
}

void Thumb::OnPointerReleased(XUI::Interfaces::PointerReleasedEventArgs& e)
{
    e.Device->Capture(nullptr);
    _lastPoint = Core::Media::Point::Zero();
    _dragStarted = false;

    Interfaces::VectorEventArgs ev;
    ev.RoutedEvent = DragCompletedEvent.get();
    ev.Vector = e.GetPosition(this);

    RaiseEvent(ev);

    e.Handled = true; // wpf never report for parents
}

