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

#include "UI/Controls/InputElement.h"
#include "UI/Controls/Control.h"

#include "Core/Dependencies/DependencyPropertyMgr.h"
#include "Core/Input/Cursors.h"
#include "Core/Input/Gestures.h"
#include "Core/GlobalInitializerScheduler.h"
#include "Platform/UIService.h"
#include "Platform/UIRenderer.h"

#include "Reflection/Runtime/Type.h"

#include "cpplinq/linq.hpp"

#include <stack>

using namespace XUI;
using namespace XUI::Interfaces;
using namespace XUI::Core::Observer;
using namespace XUI::UI::Controls;

extern void Static_Gestures();

STATIC_CLASS_PRE_INITIALIZER(InputElement, InputElement::StaticClassInitializer)

Core::Dependency::RefDependencyProperty     XUI::UI::Controls::InputElement::FocusableProperty;
Core::Dependency::RefDependencyProperty     XUI::UI::Controls::InputElement::IsEnabledProperty;
Core::Dependency::RefDependencyProperty     XUI::UI::Controls::InputElement::IsEnabledCoreProperty;
Core::Dependency::RefDependencyProperty     XUI::UI::Controls::InputElement::CursorProperty;
Core::Dependency::RefDependencyProperty     XUI::UI::Controls::InputElement::IsFocusedProperty;
Core::Dependency::RefDependencyProperty     XUI::UI::Controls::InputElement::IsHitTestVisibleProperty;
Core::Dependency::RefDependencyProperty     XUI::UI::Controls::InputElement::IsPointerOverProperty;
Core::Dependency::RefDependencyProperty     XUI::UI::Controls::InputElement::IsKeyboardFocusedProperty;
Core::Dependency::RefDependencyProperty     XUI::UI::Controls::InputElement::IsMouseOverProperty;

std::shared_ptr<Core::Observer::RoutedEvent> InputElement::GotFocusEvent;
std::shared_ptr<Core::Observer::RoutedEvent> InputElement::LostFocusEvent;
std::shared_ptr<Core::Observer::RoutedEvent> InputElement::KeyDownEvent;
std::shared_ptr<Core::Observer::RoutedEvent> InputElement::KeyUpEvent;
std::shared_ptr<Core::Observer::RoutedEvent> InputElement::TextInputEvent;
std::shared_ptr<Core::Observer::RoutedEvent> InputElement::PointerEnterEvent;
std::shared_ptr<Core::Observer::RoutedEvent> InputElement::PointerLeaveEvent;
std::shared_ptr<Core::Observer::RoutedEvent> InputElement::PointerMovedEvent;
std::shared_ptr<Core::Observer::RoutedEvent> InputElement::PointerPressedEvent;
std::shared_ptr<Core::Observer::RoutedEvent> InputElement::PointerReleasedEvent;
std::shared_ptr<Core::Observer::RoutedEvent> InputElement::PointerWheelChangedEvent;
std::shared_ptr<Core::Observer::RoutedEvent> InputElement::TappedEvent;
std::shared_ptr<Core::Observer::RoutedEvent> InputElement::DoubleTappedEvent;

void XUI::UI::Controls::InputElement::StaticClassInitializer()
{
    FocusableProperty = (Core::Dependency::BindProperty<bool>::Register<XUI::UI::Controls::InputElement>(nameof(Focusable)));
    IsEnabledProperty = (Core::Dependency::BindProperty<bool>::Register<XUI::UI::Controls::InputElement>(nameof(IsEnabled), true));
    IsEnabledCoreProperty = (Core::Dependency::BindProperty<bool>::Register<XUI::UI::Controls::InputElement>(nameof(IsEnabledCore), true));
    CursorProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::ICursor>>::Register<XUI::UI::Controls::InputElement>(nameof(Cursor), nullptr, true));
    IsFocusedProperty = (Core::Dependency::DirectProperty<XUI::UI::Controls::InputElement, bool>::RegisterDirect(nameof(IsFocused), [](XUI::UI::Controls::InputElement* x) {return x->IsFocused; }, nullptr));
    IsHitTestVisibleProperty = (Core::Dependency::BindProperty<bool>::Register<XUI::UI::Controls::InputElement>(nameof(IsHitTestVisible), true));
    IsPointerOverProperty = (Core::Dependency::DirectProperty<XUI::UI::Controls::InputElement, bool>::RegisterDirect(nameof(IsPointerOver), [](XUI::UI::Controls::InputElement* x) {return x->IsPointerOver; }, nullptr));
    IsKeyboardFocusedProperty = (Core::Dependency::BindProperty<bool>::Register<XUI::UI::Controls::InputElement>(nameof(IsKeyboardFocused)));
    IsMouseOverProperty = (Core::Dependency::DirectProperty<XUI::UI::Controls::InputElement, bool>::RegisterDirect(nameof(IsMouseOver), [](XUI::UI::Controls::InputElement* x) {return x->IsPointerOver; }, [](XUI::UI::Controls::InputElement* x, bool value) { x->IsPointerOver = value; }));

    GotFocusEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::Controls::InputElement, GotFocusEventArgs>(nameof(GotFocus), Core::Observer::RoutingStrategies::Bubble));
    LostFocusEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::Controls::InputElement, Core::Observer::RoutedEventArgs>(nameof(LostFocus), Core::Observer::RoutingStrategies::Bubble));
    KeyDownEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::Controls::InputElement, KeyEventArgs>(nameof(KeyDown), RoutingStrategies(RoutingStrategies::Tunnel | RoutingStrategies::Bubble)));
    KeyUpEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::Controls::InputElement, KeyEventArgs>(nameof(KeyUp), RoutingStrategies(RoutingStrategies::Tunnel | RoutingStrategies::Bubble)));
    TextInputEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::Controls::InputElement, TextInputEventArgs>(nameof(TextInput), RoutingStrategies(RoutingStrategies::Tunnel | RoutingStrategies::Bubble)));
    PointerEnterEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::Controls::InputElement, PointerEventArgs>(nameof(PointerEnter), RoutingStrategies(RoutingStrategies::Direct)));
    PointerLeaveEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::Controls::InputElement, PointerEventArgs>(nameof(PointerLeave), RoutingStrategies(RoutingStrategies::Direct)));
    PointerMovedEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::Controls::InputElement, PointerEventArgs>(nameof(PointerMove), RoutingStrategies(RoutingStrategies::Tunnel | RoutingStrategies::Bubble)));
    PointerPressedEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::Controls::InputElement, PointerPressedEventArgs>(nameof(PointerPressed), RoutingStrategies(RoutingStrategies::Tunnel | RoutingStrategies::Bubble)));
    PointerReleasedEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::Controls::InputElement, PointerReleasedEventArgs>(nameof(PointerReleased), RoutingStrategies(RoutingStrategies::Tunnel | RoutingStrategies::Bubble)));
    PointerWheelChangedEvent = (Core::Observer::RoutedEvent::Register<XUI::UI::Controls::InputElement, PointerWheelEventArgs>(nameof(PointerWheelChanged), RoutingStrategies(RoutingStrategies::Tunnel | RoutingStrategies::Bubble)));
    
    Static_Gestures();

    TappedEvent = (Core::Input::Gestures::TappedEvent);
    DoubleTappedEvent = (Core::Input::Gestures::DoubleTappedEvent);

    IsEnabledProperty->Changed.get_observable().subscribe(IsEnabledChanged);

    GotFocusEvent->AddClassSpecHandler<GotFocusEventArgs, InputElement>(System::make_eventhandler([](void* x, GotFocusEventArgs& e) { static_cast<InputElement*>((System::Reflection::Object*)x)->OnGotFocus(e); }));
    LostFocusEvent->AddClassSpecHandler<Core::Observer::RoutedEventArgs, InputElement>(System::make_eventhandler([](void* x, Core::Observer::RoutedEventArgs& e) { static_cast<InputElement*>((System::Reflection::Object*)x)->OnLostFocus(e); }));
    KeyDownEvent->AddClassSpecHandler<KeyEventArgs, InputElement>(System::make_eventhandler([](void* x, KeyEventArgs& e) { static_cast<InputElement*>((System::Reflection::Object*)x)->OnKeyDown(e); }));
    KeyUpEvent->AddClassSpecHandler<KeyEventArgs, InputElement>(System::make_eventhandler([](void* x, KeyEventArgs& e) { static_cast<InputElement*>((System::Reflection::Object*)x)->OnKeyUp(e); }));
    TextInputEvent->AddClassSpecHandler<TextInputEventArgs, InputElement>(System::make_eventhandler([](void* x, TextInputEventArgs& e) { static_cast<InputElement*>((System::Reflection::Object*)x)->OnTextInput(e); }));

	KeyDownEvent->AddClassSpecHandler<KeyEventArgs, InputElement>(System::make_eventhandler([](void* x, KeyEventArgs& e) { static_cast<InputElement*>((System::Reflection::Object*)x)->OnPreviewKeyDown(e); }), Core::Observer::RoutingStrategies::Tunnel);
	KeyUpEvent->AddClassSpecHandler<KeyEventArgs, InputElement>(System::make_eventhandler([](void* x, KeyEventArgs& e) { static_cast<InputElement*>((System::Reflection::Object*)x)->OnPreviewKeyUp(e); }), Core::Observer::RoutingStrategies::Tunnel);

    PointerEnterEvent->AddClassSpecHandler<PointerEventArgs, InputElement>(System::make_eventhandler([](void* x, PointerEventArgs& e)
    {
        static_cast<InputElement*>((System::Reflection::Object*)x)->OnPointerEnter(e);
    }));
    PointerLeaveEvent->AddClassSpecHandler<PointerEventArgs, InputElement>(System::make_eventhandler([](void* x, PointerEventArgs& e)
    {
        auto root = static_cast<InputElement*>((System::Reflection::Object*)x)->VisualRoot;
        if (root && root->ToVisual()->GetDrawingContext())
            root->ToVisual()->GetDrawingContext()->SetCursor(XUI::Core::Input::Cursor::Default.PlatformCursor);
        else
            XUI::Platform::UIService::Instance()->GetCache<XUI::Platform::UIRender>()->GetDrawingContext()->SetCursor(XUI::Core::Input::Cursor::Default.PlatformCursor);
        static_cast<InputElement*>((System::Reflection::Object*)x)->OnPointerLeave(e);
    }));
    PointerMovedEvent->AddClassSpecHandler<PointerEventArgs, InputElement>(System::make_eventhandler([](void* x, PointerEventArgs& e) { static_cast<InputElement*>((System::Reflection::Object*)x)->OnPointerMoved(e); }));
    PointerPressedEvent->AddClassSpecHandler<PointerPressedEventArgs, InputElement>(System::make_eventhandler([](void* x, PointerPressedEventArgs& e) { static_cast<InputElement*>((System::Reflection::Object*)x)->OnPointerPressed(e); }));
    PointerReleasedEvent->AddClassSpecHandler<PointerReleasedEventArgs, InputElement>(System::make_eventhandler([](void* x, PointerReleasedEventArgs& e) { static_cast<InputElement*>((System::Reflection::Object*)x)->OnPointerReleased(e); }));
    PointerWheelChangedEvent->AddClassSpecHandler<PointerWheelEventArgs, InputElement>(System::make_eventhandler([](void* x, PointerWheelEventArgs& e) { static_cast<InputElement*>((System::Reflection::Object*)x)->OnPointerWheelChanged(e); }));

    PointerEnterEvent->AddClassSpecHandler<PointerEventArgs, InputElement>(System::make_eventhandler([](void* x, PointerEventArgs& e){
        static_cast<InputElement*>((System::Reflection::Object*)x)->OnPreviewPointerEnter(e);
    }), Core::Observer::RoutingStrategies::Tunnel);

    PointerLeaveEvent->AddClassSpecHandler<PointerEventArgs, InputElement>(System::make_eventhandler([](void* x, PointerEventArgs& e) {
        static_cast<InputElement*>((System::Reflection::Object*)x)->OnPreviewPointerLeave(e);
    }), Core::Observer::RoutingStrategies::Tunnel);

    PointerMovedEvent->AddClassSpecHandler<PointerEventArgs, InputElement>(System::make_eventhandler([](void* x, PointerEventArgs& e) {
        static_cast<InputElement*>((System::Reflection::Object*)x)->OnPreviewPointerMoved(e);
    }), Core::Observer::RoutingStrategies::Tunnel);

    PointerPressedEvent->AddClassSpecHandler<PointerPressedEventArgs, InputElement>(System::make_eventhandler([](void* x, PointerPressedEventArgs& e) {
        static_cast<InputElement*>((System::Reflection::Object*)x)->OnPreviewPointerPressed(e);
    }), Core::Observer::RoutingStrategies::Tunnel);

    PointerReleasedEvent->AddClassSpecHandler<PointerReleasedEventArgs, InputElement>(System::make_eventhandler([](void* x, PointerReleasedEventArgs& e) {
        static_cast<InputElement*>((System::Reflection::Object*)x)->OnPreviewPointerReleased(e);
    }), Core::Observer::RoutingStrategies::Tunnel);

    PointerWheelChangedEvent->AddClassSpecHandler<PointerWheelEventArgs, InputElement>(System::make_eventhandler([](void* x, PointerWheelEventArgs& e) {
        static_cast<InputElement*>((System::Reflection::Object*)x)->OnPreviewPointerWheelChanged(e);
    }), Core::Observer::RoutingStrategies::Tunnel);
}

XUI::UI::Controls::InputElement::InputElement()
    : Visual()
    , GotFocus(this)
    , LostFocus(this)
    , KeyDown(this)
    , KeyUp(this)
    , TextInput(this)
    , PointerEnter(this)
    , PointerLeave(this)
    , PointerMoved(this)
    , PointerPressed(this)
    , PointerReleased(this)
    , PointerWheelChanged(this)
    , Tapped(this)
    , DoubleTapped(this)
    , _isFocused(false)
    , _isPointerOver(false)
    , PreviewPointerEnter(this)
    , PreviewPointerLeave(this)
    , PreviewPointerMoved(this)
    , PreviewPointerPressed(this)
    , PreviewPointerReleased(this)
    , PreviewPointerWheelChanged(this)
{
}

XUI::UI::Controls::InputElement::~InputElement()
{
}

bool XUI::UI::Controls::InputElement::GetIsKeyboardFocused() const
{
    return GetValue<bool>(IsKeyboardFocusedProperty.get());
}

void XUI::UI::Controls::InputElement::SetIsKeyboardFocused(bool value)
{
    SetValue(IsKeyboardFocusedProperty.get(), value);
}

bool XUI::UI::Controls::InputElement::GetIsMouseOver() const
{
    return _isPointerOver;
}

void XUI::UI::Controls::InputElement::SetIsMouseOver(bool value)
{
    SetAndRaise(IsMouseOverProperty.get(), _isPointerOver, value);
}

bool XUI::UI::Controls::InputElement::GetFocusable() const
{
    return GetValue<bool>(FocusableProperty.get());
}

void XUI::UI::Controls::InputElement::SetFocusable(bool value)
{
    SetValue(FocusableProperty.get(), value);
}

bool XUI::UI::Controls::InputElement::GetIsFocused() const
{
    return _isFocused;
}

void XUI::UI::Controls::InputElement::SetIsFocused(bool value)
{
    SetAndRaise(IsFocusedProperty.get(), _isFocused, value);
}

bool XUI::UI::Controls::InputElement::GetIsPointerOver() const
{
    return _isPointerOver;
}

void XUI::UI::Controls::InputElement::SetIsPointerOver(bool value)
{
    SetAndRaise(IsPointerOverProperty.get(), _isPointerOver, value);
}

bool XUI::UI::Controls::InputElement::GetIsEnabledCore() const
{
    return GetValue<bool>(IsEnabledCoreProperty.get());
}

void XUI::UI::Controls::InputElement::SetIsEnabledCore(bool value)
{
    SetValue(IsEnabledCoreProperty.get(), value);
}

bool XUI::UI::Controls::InputElement::GetIsEnabled() const
{
    return GetValue<bool>(IsEnabledProperty.get());
}

void XUI::UI::Controls::InputElement::SetIsEnabled(bool value)
{
    SetValue(IsEnabledProperty.get(), value);
}

std::shared_ptr<Interfaces::ICursor> XUI::UI::Controls::InputElement::GetCursor() const
{
    return GetValue<std::shared_ptr<Interfaces::ICursor>>(CursorProperty.get());
}

void XUI::UI::Controls::InputElement::SetCursor(std::shared_ptr<Interfaces::ICursor> const& value)
{
    SetValue(CursorProperty.get(), value);
}

bool XUI::UI::Controls::InputElement::GetIsHitTestVisible() const
{
    return GetValue<bool>(IsHitTestVisibleProperty.get());
}

void XUI::UI::Controls::InputElement::SetIsHitTestVisible(bool value)
{
    SetValue(IsHitTestVisibleProperty.get(), value);
}


/// <summary>
/// Focuses the control.
/// </summary>

void XUI::UI::Controls::InputElement::Focus()
{
    XUI::Platform::UIRender* service = XUI::Platform::UIService::Instance()->Get<XUI::Platform::UIRender>();
    service->Focus(this);
}

void XUI::UI::Controls::InputElement::IsEnabledChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
{
    ((InputElement*)e.Sender)->UpdateIsEnabledCore();
}

/// <summary>
/// Updates the <see cref="IsEnabledCore"/> property value.
/// </summary>

void XUI::UI::Controls::InputElement::UpdateIsEnabledCore()
{
    UpdateIsEnabledCore(GetVisualParent<InputElement>());
}

/// <summary>
/// Updates the <see cref="IsEnabledCore"/> property based on the parent's
/// <see cref="IsEnabledCore"/>.
/// </summary>
/// <param name="parent">The parent control.</param>

void XUI::UI::Controls::InputElement::UpdateIsEnabledCore(InputElement * parent)
{
    if (parent != nullptr)
    {
        IsEnabledCore = IsEnabled && parent->IsEnabledCore;
    }
    else
    {
        IsEnabledCore = IsEnabled;
    }

    for (auto const& child : VisualChildren.GetContainer())
    {
        auto visual = child.get();
        if (visual && *visual->GetType() == typeid(InputElement))
            static_cast<InputElement*>(visual)->UpdateIsEnabledCore(this);
    }
}


/// <summary>
/// Adds a handler for the specified routed event.
/// </summary>
/// <param name="routedEvent">The routed event.</param>
/// <param name="handler">The handler.</param>
/// <param name="routes">The routing strategies to listen to.</param>
/// <param name="handledEventsToo">Whether handled events should also be listened for.</param>
/// <returns>A disposable that terminates the event subscription.</returns>
rxcpp::subscription XUI::UI::Controls::InputElement::AddHandler(RoutedEvent* routedEvent, System::Events::IEventHook* handler, RoutingStrategies routes, bool handledEventsToo)
{
    System::ThrowIfFailed<System::ArgumentNullException>(routedEvent != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(handler != nullptr);

    auto event_handler = std::make_shared<Core::Observer::RoutedEventHandler>(handler, routes, handledEventsToo);
    auto& events = _eventHandlers[routedEvent];

    events.emplace_back(event_handler);

    std::weak_ptr<Core::Observer::RoutedEventHandler> weak_event_handler = event_handler;
    return rxcpp::make_subscription([this, routedEvent, weak_event_handler]()
    {
        if (!weak_event_handler.expired())
            _eventHandlers[routedEvent].remove(weak_event_handler.lock());
    });
}

rxcpp::subscription XUI::UI::Controls::InputElement::AddHandler(RoutedEvent* routedEvent, std::shared_ptr<System::Events::IEventHook> const& handler, RoutingStrategies routes, bool handledEventsToo)
{
    System::ThrowIfFailed<System::ArgumentNullException>(routedEvent != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(handler != nullptr);

    auto event_handler = std::make_shared<Core::Observer::RoutedEventHandler>(handler, routes, handledEventsToo);
    auto& events = _eventHandlers[routedEvent];

    events.emplace_back(event_handler);

    std::weak_ptr<Core::Observer::RoutedEventHandler> weak_event_handler = event_handler;
    return rxcpp::make_subscription([this, routedEvent, weak_event_handler]()
    {
        if (!weak_event_handler.expired())
            _eventHandlers[routedEvent].remove(weak_event_handler.lock());
    });
}

/// <summary>
/// Removes a handler for the specified routed event.
/// </summary>
/// <param name="routedEvent">The routed event.</param>
/// <param name="handler">The handler.</param>
void XUI::UI::Controls::InputElement::RemoveHandler(Core::Observer::RoutedEvent* routedEvent, System::Events::IEventHook* handler)
{
    System::ThrowIfFailed<System::ArgumentNullException>(routedEvent != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(handler != nullptr);

    auto iEvents = _eventHandlers.find(routedEvent);
    if (iEvents != _eventHandlers.end())
    {
        for (auto isub = iEvents->second.begin(); isub != iEvents->second.end();)
        {
            if (isub->get()->GetHandler()->IsEqual(handler))
                iEvents->second.erase(isub++);
            else
                ++isub;
        }
    }
}

/// <summary>
/// Raises a routed event.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Controls::InputElement::RaiseEvent(RoutedEventArgs const& e)
{
    RoutedEventArgs& _e = const_cast<RoutedEventArgs&>(e);

    if (!e.Source)
        _e.Source = this;

    if (e.RoutedEvent->RoutingStrategies == RoutingStrategies::Direct)
    {
        _e.Route = RoutingStrategies::Direct;
        RaiseEventImpl(e);
        _e.RoutedEvent->InvokeRouteFinished(_e);
    }

    if ((e.RoutedEvent->RoutingStrategies & RoutingStrategies::Tunnel) != 0)
    {
        TunnelEvent(e);
        e.RoutedEvent->InvokeRouteFinished(_e);
    }

    if ((e.RoutedEvent->RoutingStrategies & RoutingStrategies::Bubble) != 0)
    {
        BubbleEvent(e);
        e.RoutedEvent->InvokeRouteFinished(_e);
    }
}

/// <summary>
/// Bubbles an event.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Controls::InputElement::BubbleEvent(RoutedEventArgs const & e)
{
    RoutedEventArgs& _e = const_cast<RoutedEventArgs&>(e);
    _e.Route = RoutingStrategies::Bubble;

    InputElement* control = this;
    while (control != nullptr)
    {
        control->RaiseEventImpl(e);
        XUI::UI::Controls::Control* _control = control->Static_As<XUI::UI::Controls::Control>();
        if (_control)
            control = _control->Parent;
        else
            break;
    }
}

void TunnelTraverse(InputElement* _control, RoutedEventArgs const & e)
{
    if (_control)
    {
        XUI::UI::Controls::Control* control = _control->Static_As<XUI::UI::Controls::Control>();
        if (control)
            TunnelTraverse(control->Parent, e);

        _control->RaiseEventImpl(e);
    }
}

/// <summary>
/// Tunnels an event.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Controls::InputElement::TunnelEvent(RoutedEventArgs const & e)
{
    RoutedEventArgs& _e = const_cast<RoutedEventArgs&>(e);
    _e.Route = RoutingStrategies::Tunnel;

    TunnelTraverse(this, e);
}

/// <summary>
/// Carries out the actual invocation of an event on this object.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Controls::InputElement::RaiseEventImpl(RoutedEventArgs const & e)
{
    RoutedEventArgs& _e = const_cast<RoutedEventArgs&>(e);
    e.RoutedEvent->InvokeRaised(static_cast<System::Reflection::meta::Object*>(this), _e);

    auto iRouteHandler = _eventHandlers.find(e.RoutedEvent);
    if (iRouteHandler != _eventHandlers.end())
    {
        for (auto it = iRouteHandler->second.begin(); it != iRouteHandler->second.end();)
        {
            auto const& sub = *it;
            ++it;

            bool correctRoute = (e.Route & sub->Routes) != 0;
            bool notFinished = !e.Handled || sub->AlsoIfHandled;

            if (correctRoute && notFinished)
            {
                if (sub->GetHandler())
                    (*sub->GetHandler())(static_cast<System::Reflection::meta::Object*>(this), static_cast<System::Events::EventArg&>(_e));
            }
        }
    }
}

void XUI::UI::Controls::InputElement::UpdateCursor()
{
    auto cursor = Cursor;
    auto root = VisualRoot;
    if (Cursor)
    {
        if (root && root->ToVisual()->GetDrawingContext())
            root->ToVisual()->GetDrawingContext()->SetCursor(Cursor->GetPlatformCursor());
        else
            XUI::Platform::UIService::Instance()->GetCache<XUI::Platform::UIRender>()->GetDrawingContext()->SetCursor(Cursor->GetPlatformCursor());
    }
    else
    {
        if (root && root->ToVisual()->GetDrawingContext())
            root->ToVisual()->GetDrawingContext()->SetCursor(XUI::Core::Input::Cursor::Default.PlatformCursor);
        else
            XUI::Platform::UIService::Instance()->GetCache<XUI::Platform::UIRender>()->GetDrawingContext()->SetCursor(XUI::Core::Input::Cursor::Default.PlatformCursor);
    }
}

/// <inheritdoc/>
void XUI::UI::Controls::InputElement::OnDetachedFromVisualTreeCore(VisualTreeAttachmentEventArgs const & e)
{
    Visual::OnDetachedFromVisualTreeCore(e);

    if (IsFocused)
    {
        XUI::Platform::UIRender* service = XUI::Platform::UIService::Instance()->Get<XUI::Platform::UIRender>();
        service->Focus(nullptr);
    }
}

/// <inheritdoc/>
void XUI::UI::Controls::InputElement::OnAttachedToVisualTreeCore(VisualTreeAttachmentEventArgs const & e)
{
    Visual::OnAttachedToVisualTreeCore(e);
    UpdateIsEnabledCore();
}

/// <summary>
/// Called before the <see cref="GotFocus"/> event occurs.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Controls::InputElement::OnGotFocus(Interfaces::GotFocusEventArgs & e)
{
    IsFocused = e.Source == this;
}

/// <summary>
/// Called before the <see cref="LostFocus"/> event occurs.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Controls::InputElement::OnLostFocus(Core::Observer::RoutedEventArgs & e)
{
    IsFocused = false;
}

/// <summary>
/// Called before the <see cref="KeyDown"/> event occurs.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Controls::InputElement::OnKeyDown(Interfaces::KeyEventArgs & e)
{
}

/// <summary>
/// Called before the <see cref="KeyUp"/> event occurs.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Controls::InputElement::OnKeyUp(Interfaces::KeyEventArgs & e)
{
}

/// <summary>
/// Called before the <see cref="TextInput"/> event occurs.
/// </summary>
/// <param name="e">The event args.</param>

void XUI::UI::Controls::InputElement::OnTextInput(Interfaces::TextInputEventArgs & e)
{
}

/// <summary>
/// Called before the <see cref="PointerEnter"/> event occurs.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Controls::InputElement::OnPointerEnter(Interfaces::PointerEventArgs & e)
{
    UpdateCursor();
    SetAndRaise(IsMouseOverProperty.get(), _isPointerOver, true);
}

/// <summary>
/// Called before the <see cref="PointerLeave"/> event occurs.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Controls::InputElement::OnPointerLeave(Interfaces::PointerEventArgs & e)
{
    SetAndRaise(IsMouseOverProperty.get(), _isPointerOver, false);
}

/// <summary>
/// Called before the <see cref="PointerMoved"/> event occurs.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Controls::InputElement::OnPointerMoved(Interfaces::PointerEventArgs & e)
{
}

/// <summary>
/// Called before the <see cref="PointerPressed"/> event occurs.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Controls::InputElement::OnPointerPressed(Interfaces::PointerPressedEventArgs & e)
{
}

/// <summary>
/// Called before the <see cref="PointerReleased"/> event occurs.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Controls::InputElement::OnPointerReleased(Interfaces::PointerReleasedEventArgs & e)
{
}

/// <summary>
/// Called before the <see cref="PointerWheelChanged"/> event occurs.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Controls::InputElement::OnPointerWheelChanged(Interfaces::PointerWheelEventArgs & e)
{
}
