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

#include "KeyboardDevice.h"

#include "Platform/UIService.h"
#include "Platform/UIRenderer.h"
#include "UI/Controls/InputElement.h"

#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;

STATIC_CLASS_INITIALIZER(KeyboardDevice, Core::Input::KeyboardDevice::StaticClassInitializer)

Core::Dependency::RefDependencyProperty Core::Input::KeyboardDevice::DirectionalNavigationProperty;
Core::Dependency::RefDependencyProperty Core::Input::KeyboardDevice::TabNavigationProperty;
Interfaces::InputModifiers XUI::Core::Input::KeyboardDevice::Modifiers = Interfaces::InputModifiers::None;

void Core::Input::KeyboardDevice::StaticClassInitializer()
{
    DirectionalNavigationProperty = (Core::Dependency::BindProperty<Core::Input::KeyboardNavigationMode>::RegisterAttached<Core::Input::KeyboardDevice, XUI::UI::Controls::InputElement>(nameof(DirectionalNavigation), Core::Input::KeyboardNavigationMode::None));
    TabNavigationProperty = (Core::Dependency::BindProperty<Core::Input::KeyboardNavigationMode>::RegisterAttached<Core::Input::KeyboardDevice, XUI::UI::Controls::InputElement>(nameof(TabNavigation), Core::Input::KeyboardNavigationMode::Continue));
}

bool XUI::Core::Input::KeyboardDevice::Move(XUI::UI::Controls::InputElement * element, UI::Presenters::NavigationDirection direction, Interfaces::InputModifiers modifiers)
{
    System::ThrowIfFailed<System::ArgumentNullException>(element);
    
    auto next = GetNext(element, direction);
    
    if (next)
    {
        Interfaces::NavigationMethod method = direction == UI::Presenters::NavigationDirection::Next || direction == UI::Presenters::NavigationDirection::Previous ? Interfaces::NavigationMethod::Tab : Interfaces::NavigationMethod::Directional;
        SetFocusedElement(next, method, modifiers); // Raise event and set keyboard focus
        next->Focus(); // set focused element
        return true;
    }

    return false;
}

XUI::UI::Controls::InputElement * XUI::Core::Input::KeyboardDevice::GetNext(XUI::UI::Controls::InputElement * element, UI::Presenters::NavigationDirection direction)
{
    System::ThrowIfFailed<System::ArgumentNullException>(element);

    auto customHandler = element->VisitVisualAncestors([](UI::Controls::Visual* x) { return x->Is<Interfaces::UIControls::ICustomKeyboardNavigation>(); });

    if (customHandler)
    {
        bool handled = false;
        auto next = dynamic_cast<Interfaces::UIControls::ICustomKeyboardNavigation*>(customHandler)->GetNext(element, direction, handled);
    
        if (handled)
        {
            if (next)
            {
                return next;
            }
            else if (direction == UI::Presenters::NavigationDirection::Next || direction == UI::Presenters::NavigationDirection::Previous)
            {
                return TabNavigation::GetNextInTabOrder(static_cast<XUI::UI::Controls::InputElement *>(customHandler), direction, true);
            }
            else
            {
                return nullptr;
            }
        }
    }
    
    if (direction == UI::Presenters::NavigationDirection::Next || direction == UI::Presenters::NavigationDirection::Previous)
    {
        return TabNavigation::GetNextInTabOrder(element, direction);
    }
    else
    {
        return DirectionalNavigation::GetNext(element, direction);
    }
}

XUI::Core::Input::KeyboardDevice::KeyboardDevice()
{
    XUI::Platform::UIService::Instance()->Add<Interfaces::IKeyboardDevice>(this);

    _inputmgr_subcribe = XUI::Platform::UIService::Instance()->Get<Interfaces::IInputMgr>()->Process.get_observable().subscribe([this](Interfaces::RawInputEventArgs const& e)
    {
        if (e.Device == this && !e.Handled)
            KeyboardDevice::ProcessRawEvent((Interfaces::RawInputEventArgs&)e);
    });
}

XUI::Core::Input::KeyboardDevice::~KeyboardDevice()
{
    _inputmgr_subcribe.unsubscribe();
    XUI::Platform::UIService::Instance()->Remove<Interfaces::IKeyboardDevice>();
}

XUI::UI::Controls::InputElement * XUI::Core::Input::KeyboardDevice::GetFocusedElement() const { return _focusedElement.expired() ? nullptr : _focusedElement.lock().get(); }

void XUI::Core::Input::KeyboardDevice::SetFocusedElement(XUI::UI::Controls::InputElement * value)
{
    _focusedElement = value->shared_from_base_static<XUI::UI::Controls::InputElement>();
    RaisePropertyChanged(nameof(FocusedElement));
}

void XUI::Core::Input::KeyboardDevice::SetFocusedElement(XUI::UI::Controls::InputElement * element, Interfaces::NavigationMethod method, Interfaces::InputModifiers modifiers)
{
    if (element != FocusedElement)
    {
        auto interactive = FocusedElement;

        Core::Observer::RoutedEventArgs e(UI::Controls::InputElement::LostFocusEvent.get());
        interactive->RaiseEvent(e);

        FocusedElement = element;
        interactive = element;

        Interfaces::GotFocusEventArgs e_gotfocus(UI::Controls::InputElement::GotFocusEvent.get());
        e_gotfocus.NavigationMethod = method,
        e_gotfocus.InputModifiers = modifiers,

        interactive->RaiseEvent(e_gotfocus);
    }
}

void XUI::Core::Input::KeyboardDevice::ProcessRawEvent(Interfaces::RawInputEventArgs& e)
{
    auto element = FocusedElement;

    if (element != nullptr)
    {
        auto keyInput = dynamic_cast<Interfaces::RawKeyEventArgs*>(&e);

        if (keyInput != nullptr)
        {
            Modifiers = keyInput->Modifiers;

            switch (keyInput->Type)
            {
                case Interfaces::RawKeyEventType::KeyDown:
                case Interfaces::RawKeyEventType::KeyUp:
                {
                    auto routedEvent = keyInput->Type == Interfaces::RawKeyEventType::KeyDown
                        ? UI::Controls::InputElement::KeyDownEvent.get()
                        : UI::Controls::InputElement::KeyUpEvent.get();

                    Interfaces::KeyEventArgs ev(routedEvent);
                    ev.Device = this;
                    ev.Key = keyInput->Key;
                    ev.Modifiers = keyInput->Modifiers;
                    ev.Source = element;

                    //UI::Controls::Visual* currentHandler = element;
                    //while (currentHandler != nullptr && !ev.Handled && keyInput->Type == Interfaces::RawKeyEventType::KeyDown)
                    //{
                    //    auto bindings = dynamic_cast<UI::Controls::InputElement*>(currentHandler);
                    //    if (bindings != nullptr)
                    //    {
                    //        for(auto binding : bindings->KeyBindings)
                    //        {
                    //            if (ev.Handled)
                    //                break;
                    //            binding.TryHandle(ev);
                    //        }
                    //    }
                    //    currentHandler = currentHandler->VisualParent;
                    //}

                    element->RaiseEvent(ev);
                    e.Handled = ev.Handled;
                    break;
                }
            }
        }
        else
        {
            auto text = dynamic_cast<Interfaces::RawTextInputEventArgs*>(&e);

            if (text != nullptr)
            {
                Interfaces::TextInputEventArgs ev(UI::Controls::InputElement::TextInputEvent.get());
                ev.Device = this;
                ev.Text = text->Text;
                ev.Source = element;

                element->RaiseEvent(ev);
                e.Handled = ev.Handled;
            }
        }
    }
}

void XUI::Core::Input::KeyboardDevice::SetOwner(std::shared_ptr<Interfaces::UIControls::IInputRoot> const& owner)
{
    if (_scopeOwner.Ptr() == owner.get())
        return;

    if (!_scopeOwner.expired())
    {
        auto scope = _scopeOwner.lock();
        auto inputHnd = scope->ToVisualBase()->Static_As<UI::Controls::InputElement>();
        if (inputHnd)
            inputHnd->RemoveHandler(UI::Controls::InputElement::KeyDownEvent.get(), System::make_eventhandler(&KeyboardDevice::OnKeyDown, this));
    }

    if (owner)
    {
        static_cast<std::weak_ptr<Interfaces::UIControls::IInputRoot>&>(_scopeOwner) = owner;
        auto inputHnd = owner->ToVisualBase()->Static_As<UI::Controls::InputElement>();
        if (inputHnd)
            inputHnd->AddHandler(UI::Controls::InputElement::KeyDownEvent.get(), System::make_eventhandler(&KeyboardDevice::OnKeyDown, this));
    }
}

void XUI::Core::Input::KeyboardDevice::OnKeyDown(void * sender, Interfaces::KeyEventArgs & e)
{
    auto focusMgr = XUI::Platform::UIService::Instance()->GetCache<XUI::Platform::UIRender>();
    if (!focusMgr)
        return;

    auto focus = focusMgr->FocusControl.lock();

    if (focus)
    {
        UI::Presenters::NavigationDirection direction = UI::Presenters::NavigationDirection::None;

        switch (e.Key)
        {
            case Interfaces::Key::Tab:
                direction = (e.Modifiers & Interfaces::InputModifiers::Shift) == 0 ? UI::Presenters::NavigationDirection::Next : UI::Presenters::NavigationDirection::Previous;
                break;
            case Interfaces::Key::Up:
                direction = UI::Presenters::NavigationDirection::Up;
                break;
            case Interfaces::Key::Down:
                direction = UI::Presenters::NavigationDirection::Down;
                break;
            case Interfaces::Key::Left:
                direction = UI::Presenters::NavigationDirection::Left;
                break;
            case Interfaces::Key::Right:
                direction = UI::Presenters::NavigationDirection::Right;
                break;
            case Interfaces::Key::PageUp:
                direction = UI::Presenters::NavigationDirection::PageUp;
                break;
            case Interfaces::Key::PageDown:
                direction = UI::Presenters::NavigationDirection::PageDown;
                break;
            case Interfaces::Key::Home:
                direction = UI::Presenters::NavigationDirection::First;
                break;
            case Interfaces::Key::End:
                direction = UI::Presenters::NavigationDirection::Last;
                break;
        }

        if (direction != UI::Presenters::NavigationDirection::None)
        {
            if (Move(focus->Static_As<UI::Controls::InputElement>(), direction, e.Modifiers))
                e.Handled = true;
        }
    }
}

XUI::Core::Input::KeyboardNavigationMode XUI::Core::Input::KeyboardDevice::GetDirectionalNavigation(XUI::UI::Controls::InputElement * element)
{
    return element->GetValue<KeyboardNavigationMode>(DirectionalNavigationProperty);
}

void XUI::Core::Input::KeyboardDevice::SetDirectionalNavigation(XUI::UI::Controls::InputElement * element, XUI::Core::Input::KeyboardNavigationMode value)
{
    element->SetValue(DirectionalNavigationProperty, value);
}

XUI::Core::Input::KeyboardNavigationMode XUI::Core::Input::KeyboardDevice::GetTabNavigation(XUI::UI::Controls::InputElement * element)
{
    return element->GetValue<KeyboardNavigationMode>(TabNavigationProperty);
}

void XUI::Core::Input::KeyboardDevice::SetTabNavigation(XUI::UI::Controls::InputElement * element, XUI::Core::Input::KeyboardNavigationMode value)
{
    element->SetValue(TabNavigationProperty, value);
}

/// <summary>
/// Gets the next control in the specified navigation direction.
/// </summary>
/// <param name="element">The element.</param>
/// <param name="direction">The navigation direction.</param>
/// <returns>
/// The next element in the specified direction, or nullptr if <paramref name="element"/>
/// was the last in the requested direction.
/// </returns>
XUI::UI::Controls::InputElement * XUI::Core::Input::DirectionalNavigation::GetNext(XUI::UI::Controls::InputElement * element, UI::Presenters::NavigationDirection direction)
{
    System::ThrowIfFailed<System::ArgumentNullException>(element);
    System::ThrowIfFailed<System::ArgumentException>(
        direction != UI::Presenters::NavigationDirection::Next &&
        direction != UI::Presenters::NavigationDirection::Previous);

    auto container = element->GetVisualParent()->Static_As<XUI::UI::Controls::InputElement>();

    if (container)
    {
        auto mode = KeyboardDevice::GetDirectionalNavigation(container);

        switch (mode)
        {
            case KeyboardNavigationMode::Continue:
            {
                auto result = GetNextInContainer(element, container, direction);
                if (!result)
                    return GetFirstInNextContainer(element, element, direction);
                return result;
            }
            case KeyboardNavigationMode::Cycle:
            {
                auto result = GetNextInContainer(element, container, direction);
                if (!result)
                    return GetFocusableDescendant(container, direction);
                return result;
            }
            case KeyboardNavigationMode::Contained:
                return GetNextInContainer(element, container, direction);
            default:
                return nullptr;
        }
    }
    else
    {
        return static_cast<XUI::UI::Controls::InputElement*>(element->VisitVisualDescendents([](UI::Controls::Visual* x)
        {
            auto element = x->Static_As<XUI::UI::Controls::InputElement>();
            if (element)
                return element->Focusable && element->IsEnabledCore && element->IsVisible;
            return false;
        }));
    }
}

/// <summary>
/// Returns a value indicting whether the specified direction is forward.
/// </summary>
/// <param name="direction">The direction.</param>
/// <returns>True if the direction is forward.</returns>

bool XUI::Core::Input::DirectionalNavigation::IsForward(UI::Presenters::NavigationDirection direction)
{
    return direction == UI::Presenters::NavigationDirection::Next ||
        direction == UI::Presenters::NavigationDirection::Last ||
        direction == UI::Presenters::NavigationDirection::Right ||
        direction == UI::Presenters::NavigationDirection::Down;
}

/// <summary>
/// Gets the first or last focusable descendant of the specified element.
/// </summary>
/// <param name="container">The element.</param>
/// <param name="direction">The direction to search.</param>
/// <returns>The element or nullptr if not found.##</returns>

XUI::UI::Controls::InputElement * XUI::Core::Input::DirectionalNavigation::GetFocusableDescendant(XUI::UI::Controls::InputElement * container, UI::Presenters::NavigationDirection direction)
{
    return IsForward(direction) ? GetFocusableDescendants(container, false) : GetFocusableDescendants(container, true);
}

/// <summary>
/// Gets the focusable descendants of the specified element.
/// </summary>
/// <param name="element">The element.</param>
/// <returns>The element's focusable descendants.</returns>

XUI::UI::Controls::InputElement * XUI::Core::Input::DirectionalNavigation::GetFocusableDescendants(XUI::UI::Controls::InputElement * element, bool last)
{
    if (!last)
    {
        return static_cast<XUI::UI::Controls::InputElement*>(element->VisitVisualDescendents([element](UI::Controls::Visual* x)
        {
            auto _element = x->Static_As<XUI::UI::Controls::InputElement>();
            if (_element && element != _element)
                return _element->Focusable && _element->IsEnabledCore && _element->IsVisible;
            return false;
        }));
    }

    XUI::UI::Controls::InputElement * result = nullptr;
    for (auto child : element->GetVisualChildren().GetNativeEnumerator())
    {
        auto _element = child->Static_As<XUI::UI::Controls::InputElement>();
        if (_element && _element->IsEnabledCore && _element->IsVisible)
        {
            auto res = GetFocusableDescendants(_element, last);
            if (res)
                result = res;
            else if (_element->Focusable)
                result = _element;
        }
    }

    return result;
}

/// <summary>
/// Gets the next item that should be focused in the specified container.
/// </summary>
/// <param name="element">The starting element/</param>
/// <param name="container">The container.</param>
/// <param name="direction">The direction.</param>
/// <returns>The next element, or nullptr if the element is the last.</returns>

XUI::UI::Controls::InputElement * XUI::Core::Input::DirectionalNavigation::GetNextInContainer(XUI::UI::Controls::InputElement * element, XUI::UI::Controls::InputElement * container, UI::Presenters::NavigationDirection direction)
{
    if (direction == UI::Presenters::NavigationDirection::Down)
    {
        auto descendant = GetFocusableDescendants(element, false);

        if (descendant)
        {
            return descendant;
        }
    }

    if (container)
    {
        auto navigable = container->Dynamic_As<Interfaces::UIControls::INavigableContainer>();

        if (navigable)
        {
            while (element)
            {
                element = navigable->GetControl(direction, element);

                if (element && element->Focusable && element->IsEnabledCore && element->IsVisible)
                    break;
            }
        }
        else
        {
            // TODO: Do a spatial search here if the container doesn't implement
            // INavigableContainer.
            element = nullptr;
        }

        if (element && direction == UI::Presenters::NavigationDirection::Up)
        {
            auto descendant = GetFocusableDescendants(element, true);

            if (descendant)
            {
                return descendant;
            }
        }

        return element;
    }

    return nullptr;
}

XUI::UI::Controls::InputElement * XUI::Core::Input::DirectionalNavigation::GetFirstInNextContainer(XUI::UI::Controls::InputElement * element, XUI::UI::Controls::InputElement * container, UI::Presenters::NavigationDirection direction)
{
    auto parent = container->GetVisualParent<XUI::UI::Controls::InputElement>();
    auto isForward = IsForward(direction);
    XUI::UI::Controls::InputElement* next = nullptr;

    if (parent)
    {
        if (!isForward && parent->Focusable && parent->IsEnabledCore && parent->IsVisible)
        {
            return parent;
        }

        std::list<XUI::UI::Controls::InputElement*> siblings;
        for (auto child : parent->GetVisualChildren().GetNativeEnumerator())
        {
            auto x = child->Static_As<XUI::UI::Controls::InputElement>();
            if (x->IsEnabledCore && x->IsVisible)
            {
                if (isForward && x != container)
                    continue;

                if (!isForward && x == container)
                    continue;

                siblings.push_back(x);
            }
        }

        if (!siblings.empty())
        {
            XUI::UI::Controls::InputElement* sibling = nullptr;

            if (isForward)
            {
                sibling = *siblings.begin();
            }
            else
            {
                sibling = *siblings.rbegin();
            }

            Interfaces::UIControls::ICustomKeyboardNavigation* custom = sibling->Dynamic_As<Interfaces::UIControls::ICustomKeyboardNavigation>();
            if (custom)
            {
                bool handled = false;
                next = custom->GetNext(element, direction, handled);

                if (handled)
                {
                    return next;
                }
            }

            if (sibling->Focusable && sibling->IsEnabledCore && sibling->IsVisible)
            {
                next = sibling;
            }
            else
            {
                next = isForward ? GetFocusableDescendants(sibling, false) : GetFocusableDescendants(sibling, true);
            }
        }

        if (next == nullptr)
        {
            next = GetFirstInNextContainer(element, parent, direction);
        }
    }
    else
    {
        next = isForward ? GetFocusableDescendants(container, false) : GetFocusableDescendants(container, true);
    }

    return next;
}

/// <summary>
/// Gets the next control in the specified tab direction.
/// </summary>
/// <param name="element">The element.</param>
/// <param name="direction">The tab direction. Must be Next or Previous.</param>
/// <param name="outsideElement">
/// If true will not descend into <paramref name="element"/> to find next control.
/// </param>
/// <returns>
/// The next element in the specified direction, or nullptr if <paramref name="element"/>
/// was the last in the requested direction.
/// </returns>

XUI::UI::Controls::InputElement * XUI::Core::Input::TabNavigation::GetNextInTabOrder(XUI::UI::Controls::InputElement * element, UI::Presenters::NavigationDirection direction, bool outsideElement)
{
    System::ThrowIfFailed<System::ArgumentNullException>(element);
    System::ThrowIfFailed<System::ArgumentException>(
        direction == UI::Presenters::NavigationDirection::Next ||
        direction == UI::Presenters::NavigationDirection::Previous);

    auto container = element->GetVisualParent<XUI::UI::Controls::InputElement>();

    if (container)
    {
        auto mode = KeyboardDevice::GetTabNavigation(container);

        switch (mode)
        {
            case KeyboardNavigationMode::Continue:
            {
                auto result = GetNextInContainer(element, container, direction, outsideElement);
                if (!result)
                    return GetFirstInNextContainer(element, element, direction);
                return result;
            }
            case KeyboardNavigationMode::Cycle:
            {
                auto result = GetNextInContainer(element, container, direction, outsideElement);
                if (!result)
                    return GetFocusableDescendant(container, direction);
                return result;
            }
            case KeyboardNavigationMode::Contained:
                return GetNextInContainer(element, container, direction, outsideElement);
            default:
                return GetFirstInNextContainer(element, container, direction);
        }
    }
    else
    {
        return GetFocusableDescendants(element, direction, false);
    }
}

/// <summary>
/// Gets the first or last focusable descendant of the specified element.
/// </summary>
/// <param name="container">The element.</param>
/// <param name="direction">The direction to search.</param>
/// <returns>The element or nullptr if not found.##</returns>

XUI::UI::Controls::InputElement * XUI::Core::Input::TabNavigation::GetFocusableDescendant(XUI::UI::Controls::InputElement * container, UI::Presenters::NavigationDirection direction)
{
    return direction == UI::Presenters::NavigationDirection::Next ? GetFocusableDescendants(container, direction, false) : GetFocusableDescendants(container, direction, true);
}

/// <summary>
/// Gets the focusable descendants of the specified element.
/// </summary>
/// <param name="element">The element.</param>
/// <param name="direction">The tab direction. Must be Next or Previous.</param>
/// <returns>The element's focusable descendants.</returns>

XUI::UI::Controls::InputElement * XUI::Core::Input::TabNavigation::GetFocusableDescendants(XUI::UI::Controls::InputElement * element, UI::Presenters::NavigationDirection direction, bool last)
{
    auto mode = KeyboardDevice::GetTabNavigation(element);
    
    if (mode == KeyboardNavigationMode::None)
    {
        return nullptr;
    }
    
    std::list<XUI::UI::Controls::InputElement*> children;
    for (auto child : element->GetVisualChildren().GetNativeEnumerator())
    {
        auto x = child->Static_As<XUI::UI::Controls::InputElement>();
        if (x->IsEnabledCore && x->IsVisible)
        {
            children.push_back(x);
        }
    }

    //if (mode == KeyboardNavigationMode::Once)
    //{
    //    auto active = KeyboardDevice::GetTabOnceActiveElement(element);
    //
    //    if (active)
    //    {
    //        return active;
    //    }
    //    else
    //    {
    //        children = children.Take(1);
    //    }
    //}

    XUI::UI::Controls::InputElement * result = nullptr;
    for (auto child : children)
    {
        //    auto customNext = GetCustomNext(child, direction);
        //
        //    if (customNext.handled)
        //    {
        //        return customNext.next;
        //    }

        if (child->Focusable)
        {
            if (!last)
                return child;
        }

        auto res = GetFocusableDescendants(child, direction, last);
        if (res)
            result = res;
        else if (child->Focusable)
            result = child;
    }

    return result;
}

/// <summary>
/// Gets the next item that should be focused in the specified container.
/// </summary>
/// <param name="element">The starting element/</param>
/// <param name="container">The container.</param>
/// <param name="direction">The direction.</param>
/// <param name="outsideElement">
/// If true will not descend into <paramref name="element"/> to find next control.
/// </param>
/// <returns>The next element, or nullptr if the element is the last.</returns>

XUI::UI::Controls::InputElement * XUI::Core::Input::TabNavigation::GetNextInContainer(XUI::UI::Controls::InputElement * element, XUI::UI::Controls::InputElement * container, UI::Presenters::NavigationDirection direction, bool outsideElement)
{
    if (direction == UI::Presenters::NavigationDirection::Next && !outsideElement)
    {
        auto descendant = GetFocusableDescendants(element, direction, false);

        if (descendant)
        {
            return descendant;
        }
    }

    if (container)
    {
        auto navigable = container->Dynamic_As<Interfaces::UIControls::INavigableContainer>();

        if (navigable)
        {
            while (element)
            {
                element = navigable->GetControl(direction, element);

                if (element && element->Focusable && element->IsEnabledCore && element->IsVisible)
                    break;
            }
        }
        else
        {
            // TODO: Do a spatial search here if the container doesn't implement
            // INavigableContainer.
            element = nullptr;
        }

        if (element && direction == UI::Presenters::NavigationDirection::Up)
        {
            auto descendant = GetFocusableDescendants(element, direction, true);

            if (descendant)
            {
                return descendant;
            }
        }

        return element;
    }

    return nullptr;
}

XUI::UI::Controls::InputElement * XUI::Core::Input::TabNavigation::GetFirstInNextContainer(XUI::UI::Controls::InputElement * element, XUI::UI::Controls::InputElement * container, UI::Presenters::NavigationDirection direction)
{
    auto parent = container->GetVisualParent<XUI::UI::Controls::InputElement>();
    XUI::UI::Controls::InputElement* next = nullptr;

    if (parent)
    {
        if (direction == UI::Presenters::NavigationDirection::Previous && parent->Focusable && parent->IsEnabledCore && parent->IsVisible)
        {
            return parent;
        }

        std::list<XUI::UI::Controls::InputElement*> siblings;
        for (auto child : parent->GetVisualChildren().GetNativeEnumerator())
        {
            auto x = child->Static_As<XUI::UI::Controls::InputElement>();
            if (x->IsEnabledCore && x->IsVisible)
            {
                if (direction == UI::Presenters::NavigationDirection::Next && x != container)
                    continue;

                if (direction == UI::Presenters::NavigationDirection::Previous && x == container)
                    continue;

                siblings.push_back(x);
            }
        }

        if (!siblings.empty())
        {
            XUI::UI::Controls::InputElement* sibling = nullptr;

            if (direction == UI::Presenters::NavigationDirection::Next)
            {
                sibling = *siblings.begin();
            }
            else
            {
                sibling = *siblings.rbegin();
            }

            Interfaces::UIControls::ICustomKeyboardNavigation* custom = sibling->Dynamic_As<Interfaces::UIControls::ICustomKeyboardNavigation>();
            if (custom)
            {
                bool handled = false;
                next = custom->GetNext(element, direction, handled);

                if (handled)
                {
                    return next;
                }
            }

            if (sibling->Focusable && sibling->IsEnabledCore && sibling->IsVisible)
            {
                next = sibling;
            }
            else
            {
                next = direction == UI::Presenters::NavigationDirection::Next ? GetFocusableDescendants(sibling, direction, false) : GetFocusableDescendants(sibling, direction, true);
            }
        }

        if (next == nullptr)
        {
            next = GetFirstInNextContainer(element, parent, direction);
        }
    }
    else
    {
        next = direction == UI::Presenters::NavigationDirection::Next ? GetFocusableDescendants(container, direction, false) : GetFocusableDescendants(container, direction, true);
    }

    return next;
}
