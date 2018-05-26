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

#include "MouseDevice.h"

#include "Platform/UIService.h"
#include "Platform/UIRenderer.h"
#include "UI/Controls/Control.h"

using namespace XUI;

Interfaces::InputModifiers XUI::Core::Input::MouseDevice::Modifiers = Interfaces::InputModifiers::None;

/// <summary>
/// Intializes a new instance of <see cref="MouseDevice"/>.
/// </summary>

XUI::Core::Input::MouseDevice::MouseDevice()
{
    XUI::Platform::UIService::Instance()->Add<Interfaces::IMouseDevice>(this);

    _inputmgr_subcribe = XUI::Platform::UIService::Instance()->Get<Interfaces::IInputMgr>()->Process.get_observable().subscribe([this](Interfaces::RawInputEventArgs const& e)
    {
        if (e.Device == this && !e.Handled)
        {
            Interfaces::RawMouseEventArgs& _e = (Interfaces::RawMouseEventArgs&)e;
            MouseDevice::ProcessRawEvent(_e);
        }
    });
}

XUI::Core::Input::MouseDevice::~MouseDevice()
{
    _inputmgr_subcribe.unsubscribe();
    XUI::Platform::UIService::Instance()->Remove<Interfaces::IMouseDevice>();
}

/// <summary>
/// Captures mouse input to the specified control.
/// </summary>
/// <param name="control">The control.</param>
/// <remarks>
/// When an element captures the mouse, it recieves mouse input whether the cursor is 
/// within the control's bounds or not. The current mouse capture control is exposed
/// by the <see cref="Captured"/> property.
/// </remarks>

void XUI::Core::Input::MouseDevice::Capture(UI::Controls::InputElement * control)
{
    if (control)
        _captured = control->shared_from_base_static<UI::Controls::InputElement>();
    else
        _captured.reset();
}

/// <summary>
/// Gets the mouse position relative to a control.
/// </summary>
/// <param name="relativeTo">The control.</param>
/// <returns>The mouse position in the control's coordinates.</returns>
Core::Media::Point XUI::Core::Input::MouseDevice::GetRelativePosition(UI::Controls::Visual * relativeTo)
{
    System::ThrowIfFailed<System::ArgumentNullException>(relativeTo != nullptr);

    Core::Media::Point p = Core::Media::Point::Zero();
    UI::Controls::Visual* v = relativeTo;
    UI::Controls::Visual* root = nullptr;

    while (v != nullptr)
    {
        p += (glm::vec2&)v->Bounds.x0y0();
        root = v;
        if (v->GetDrawingContext())
            break;
        v = v->VisualParent;
    }

    return root->VisualRoot->PointToClient(Position) - p;
}

void XUI::Core::Input::MouseDevice::ProcessRawEvent(Interfaces::RawMouseEventArgs & e)
{
    auto mouse = (Interfaces::IMouseDevice*)e.Device;

    _position = e.Root->PointToScreen(e.Position);

    Modifiers = e.InputModifiers;

    switch (e.Type)
    {
        case Interfaces::RawMouseEventType::LeaveWindow:
            LeaveWindow(mouse, e.Root);
            break;
        case Interfaces::RawMouseEventType::LeftButtonDown:
        case Interfaces::RawMouseEventType::RightButtonDown:
        case Interfaces::RawMouseEventType::MiddleButtonDown:
            e.Handled = MouseDown(mouse, e.Timestamp, e.Root, e.Position,
                e.Type == Interfaces::RawMouseEventType::LeftButtonDown
                ? Interfaces::MouseButton::Left
                : e.Type == Interfaces::RawMouseEventType::RightButtonDown ? Interfaces::MouseButton::Right : Interfaces::MouseButton::Middle,
                e.InputModifiers);
            break;
        case Interfaces::RawMouseEventType::LeftButtonUp:
        case Interfaces::RawMouseEventType::RightButtonUp:
        case Interfaces::RawMouseEventType::MiddleButtonUp:
            e.Handled = MouseUp(mouse, e.Root, e.Position,
                e.Type == Interfaces::RawMouseEventType::LeftButtonUp
                ? Interfaces::MouseButton::Left
                : e.Type == Interfaces::RawMouseEventType::RightButtonUp ? Interfaces::MouseButton::Right : Interfaces::MouseButton::Middle,
                e.InputModifiers);
            break;
        case Interfaces::RawMouseEventType::Move:
            e.Handled = MouseMove(mouse, e.Root, e.Position, e.InputModifiers);
            break;
        case Interfaces::RawMouseEventType::Wheel:
            e.Handled = MouseWheel(mouse, e.Root, e.Position, e.Delta, e.InputModifiers);
            break;
    }
}

void XUI::Core::Input::MouseDevice::LeaveWindow(Interfaces::IMouseDevice * device, Interfaces::UIControls::IInputRoot * root)
{
    System::ThrowIfFailed<System::ArgumentNullException>(device != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(root != nullptr);

    ClearPointerOver(this, root);
}

bool XUI::Core::Input::MouseDevice::MouseDown(Interfaces::IMouseDevice * device, uint64 timestamp, Interfaces::UIControls::IInputRoot * root, Core::Media::Point const & p, Interfaces::MouseButton button, Interfaces::InputModifiers inputModifiers)
{
    System::ThrowIfFailed<System::ArgumentNullException>(device != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(root != nullptr);

    auto hit = HitTest(root->ToVisualBase(), p);

    if (hit != nullptr)
    {
        UI::Controls::InputElement* source = GetSource(hit);

        if (source != nullptr)
        {
            auto doubleClickTime = 250; // settings.DoubleClickTime.TotalMilliseconds;

            if (!_lastClickRect.contains(p) || ((timestamp - _lastClickTime) > doubleClickTime))
            {
                _clickCount = 0;
            }

            ++_clickCount;
            _lastClickTime = timestamp;
            _lastClickRect = _lastClickRect.xywh(p.x, p.y, 10, 10);

            auto SourceGuard = source->shared_from_base_static<UI::Controls::InputElement>();

            Interfaces::PointerPressedEventArgs e(UI::Controls::InputElement::PointerPressedEvent.get());
            e.Device = device;
            e.Source = source;
            e.ClickCount = _clickCount;
            e.MouseButton = button;
            e.InputModifiers = inputModifiers;

            source->RaiseEvent(e);
            return e.Handled;
        }
    }

    return false;
}

bool XUI::Core::Input::MouseDevice::MouseMove(Interfaces::IMouseDevice * device, Interfaces::UIControls::IInputRoot * root, Core::Media::Point const & p, Interfaces::InputModifiers inputModifiers)
{
    System::ThrowIfFailed<System::ArgumentNullException>(device != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(root != nullptr);

    UI::Controls::InputElement* source;

    if (_captured.expired())
    {
        source = SetPointerOver(this, root, p);
    }
    else
    {
        SetPointerOver(this, root, Captured);
        source = Captured;
    }

    if (source)
    {
        auto SourceGuard = source->shared_from_base_static<UI::Controls::InputElement>();

        Interfaces::PointerEventArgs e(UI::Controls::InputElement::PointerMovedEvent.get());
        e.Device = device;
        e.Source = source;
        e.InputModifiers = inputModifiers;

        source->RaiseEvent(e);
        return e.Handled;
    }
    return false;
}

bool XUI::Core::Input::MouseDevice::MouseUp(Interfaces::IMouseDevice * device, Interfaces::UIControls::IInputRoot * root, Core::Media::Point const & p, Interfaces::MouseButton button, Interfaces::InputModifiers inputModifiers)
{
    System::ThrowIfFailed<System::ArgumentNullException>(device != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(root != nullptr);

    auto hit = HitTest(root->ToVisualBase(), p);

    if (hit != nullptr)
    {
        auto source = GetSource(hit);

        auto SourceGuard = source->shared_from_base_static<UI::Controls::InputElement>();

        Interfaces::PointerReleasedEventArgs e(UI::Controls::InputElement::PointerReleasedEvent.get());
        e.Device = device;
        e.Source = source;
        e.MouseButton = button;
        e.InputModifiers = inputModifiers;

        source->RaiseEvent(e);
        return e.Handled;
    }

    return false;
}

bool XUI::Core::Input::MouseDevice::MouseWheel(Interfaces::IMouseDevice * device, Interfaces::UIControls::IInputRoot * root, Core::Media::Point const & p, Core::Media::Vector const & delta, Interfaces::InputModifiers inputModifiers)
{
    System::ThrowIfFailed<System::ArgumentNullException>(device != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(root != nullptr);

    auto hit = HitTest(root->ToVisualBase(), p);

    if (hit != nullptr)
    {
        auto source = GetSource(hit);

        auto SourceGuard = source->shared_from_base_static<UI::Controls::InputElement>();

        Interfaces::PointerWheelEventArgs e(UI::Controls::InputElement::PointerWheelChangedEvent.get());
        e.Device = device;
        e.Source = source;
        e.Delta = delta;
        e.InputModifiers = inputModifiers;

        source->RaiseEvent(e);
        return e.Handled;
    }

    return false;
}

UI::Controls::InputElement * XUI::Core::Input::MouseDevice::GetSource(UI::Controls::Visual * hit)
{
    System::ThrowIfFailed<System::ArgumentNullException>(hit != nullptr);

    if (!_captured.expired())
        return Captured;

    UI::Controls::InputElement* input = dynamic_cast<UI::Controls::InputElement*>(hit);
    if (input)
        return input;

    UI::Controls::Visual* visual = hit->VisualParent;
    while (visual)
    {
        input = dynamic_cast<UI::Controls::InputElement*>(visual);
        if (input)
            return input;

        visual = hit->VisualParent;
    }

    return nullptr;
}

UI::Controls::InputElement * XUI::Core::Input::MouseDevice::HitTest(UI::Controls::Visual * root, Core::Media::Point const & p)
{
    System::ThrowIfFailed<System::ArgumentNullException>(root != nullptr);

    if (!_captured.expired())
        return Captured;

    Interfaces::IRenderRoot* RenderRoot = root->VisualRoot;
    if (!RenderRoot)
        RenderRoot = root->Dynamic_As<Interfaces::IRenderRoot>();

    if (RenderRoot)
    {
        UI::Controls::InputElement* element = nullptr;
        auto hit = RenderRoot->HitTestVisualTree(p, root, nullptr);
        if (hit)
            element = hit->Static_As<UI::Controls::InputElement>();
        return element;
    }

    return nullptr;
}

void XUI::Core::Input::MouseDevice::ClearPointerOver(Interfaces::IPointerDevice * device, Interfaces::UIControls::IInputRoot * root)
{
    System::ThrowIfFailed<System::ArgumentNullException>(device != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(root != nullptr);

    XUI::Platform::UIRender* service = XUI::Platform::UIService::Instance()->Get<XUI::Platform::UIRender>();

    auto element = !service->HoverControl.expired() ? service->HoverControl.lock()->Dynamic_As<UI::Controls::InputElement>() : nullptr;
    Interfaces::PointerEventArgs e(UI::Controls::InputElement::PointerLeaveEvent.get());
    e.Device = device;
    e.InputModifiers = Modifiers;

    while (element != nullptr)
    {
        auto SourceGuard = element->shared_from_base_static<UI::Controls::InputElement>();

        e.Source = element;
        element->RaiseEvent(e);
        if (element->VisualParent)
        {
            element = element->VisualParent->Dynamic_As<UI::Controls::InputElement>();
            continue;
        }
        break;
    }

    service->Hover(nullptr);
}

UI::Controls::InputElement * XUI::Core::Input::MouseDevice::SetPointerOver(Interfaces::IPointerDevice * device, Interfaces::UIControls::IInputRoot * root, Core::Media::Point const & p)
{
    System::ThrowIfFailed<System::ArgumentNullException>(device != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(root != nullptr);

    XUI::Platform::UIRender* service = XUI::Platform::UIService::Instance()->Get<XUI::Platform::UIRender>();

    auto element = HitTest(root->ToVisualBase(), p);

    if (element != service->HoverControl.lock().get())
    {
        if (element != nullptr)
        {
            SetPointerOver(device, root, element);
        }
        else
        {
            ClearPointerOver(device, root);
        }
    }

    return element;
}

void XUI::Core::Input::MouseDevice::SetPointerOver(Interfaces::IPointerDevice * device, Interfaces::UIControls::IInputRoot * root, UI::Controls::InputElement * element)
{
    System::ThrowIfFailed<System::ArgumentNullException>(device != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(root != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(element != nullptr);

    UI::Controls::InputElement* branch = nullptr;
    XUI::Platform::UIRender* service = XUI::Platform::UIService::Instance()->Get<XUI::Platform::UIRender>();

    Interfaces::PointerEventArgs e(UI::Controls::InputElement::PointerEnterEvent.get());
    e.Device = device;

    auto el = element;

    while (el != nullptr)
    {
        if (el->IsPointerOver)
        {
            branch = el;
            break;
        }

        auto SourceGuard = el->shared_from_base_static<UI::Controls::InputElement>();

        e.Source = el;
        e.InputModifiers = Modifiers;
        el->RaiseEvent(e);
        el = dynamic_cast<UI::Controls::InputElement*>(el->VisualParent);
    }

    el = dynamic_cast<UI::Controls::InputElement*>(service->HoverControl.lock().get());
    e.RoutedEvent = UI::Controls::InputElement::PointerLeaveEvent.get();

    while (el != nullptr && el != branch)
    {
        auto SourceGuard = el->shared_from_base_static<UI::Controls::InputElement>();

        e.Source = el;
        e.InputModifiers = Modifiers;
        el->RaiseEvent(e);
        el = dynamic_cast<UI::Controls::InputElement*>(el->VisualParent);
    }

    service->Hover(element);
}

