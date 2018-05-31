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

#ifndef __XUI_KEYBOARD_DEVICE_H__
#define __XUI_KEYBOARD_DEVICE_H__

#include "Core/Dependencies/Models/DataProperty.h"
#include "Core/Dependencies/Models/DirectProperty.h"
#include "Core/Observer/RoutedEvent.h"

#include "standard/BasicPrimities.h"

#include "Interfaces/IBrush.h"
#include "Interfaces/ICursor.h"
#include "Interfaces/InputEventArgs.h"
#include "Interfaces/IInputDevices.h"

#include <rxcpp/rx.hpp>

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/Controls/Visual.h"))
MetaAssemblyEnd

namespace XUI::UI::Controls
{
    class InputElement;
    class Visual;
}

namespace XUI::UI
{
    namespace Presenters
    {
        enum class NavigationDirection : uint8;
    }
}

namespace XUI::Interfaces::UIControls
{
    struct IInputRoot;
}

namespace XUI::Core::Input
{
    enum class KeyboardNavigationMode
    {
        /// <summary>
        /// Items in the container will be cycled through, and focus will be moved to the
        /// previous/next container after the first/last control in the container.
        /// </summary>
        Continue,

        /// <summary>
        /// Items in the container will be cycled through, and moving past the first or last
        /// control in the container will cause the last/first control to be focused.
        /// </summary>
        Cycle,

        /// <summary>
        /// Items in the container will be cycled through and focus will stop moving when the edge
        /// of the container is reached.
        /// </summary>
        Contained,

        /// <summary>
        /// When focus is moved into the container, the control described by the
        /// <see cref="KeyboardNavigation.TabOnceActiveElementProperty"/> attached property on the
        /// container will be focused. When focus moves away from this control, focus will move to
        /// the previous/next container.
        /// </summary>
        Once,

        /// <summary>
        /// The container's children will not be focused when using the tab key.
        /// </summary>
        None,
    };

    class TC_XUI_API KeyboardDevice : public Interfaces::IKeyboardDevice
    {
    private:
        std::weak_ptr<XUI::UI::Controls::InputElement> _focusedElement;
        struct ScopeOwner : std::weak_ptr<Interfaces::UIControls::IInputRoot>
        {
            Interfaces::UIControls::IInputRoot* Ptr() const
            {
                return get();
            }
        } _scopeOwner;

        rxcpp::subscription _inputmgr_subcribe;

        bool Move(
            XUI::UI::Controls::InputElement* element,
            UI::Presenters::NavigationDirection direction,
            Interfaces::InputModifiers modifiers = Interfaces::InputModifiers::None);

        static XUI::UI::Controls::InputElement* GetNext(
            XUI::UI::Controls::InputElement* element,
            UI::Presenters::NavigationDirection direction);

    public:

        static Core::Dependency::RefDependencyProperty DirectionalNavigationProperty;
        static Core::Dependency::RefDependencyProperty TabNavigationProperty;
        static Interfaces::InputModifiers Modifiers;

        static void StaticClassInitializer();

        KeyboardDevice();
        virtual ~KeyboardDevice();

        PROPERTY_(XUI::UI::Controls::InputElement*, FocusedElement);
        XUI::UI::Controls::InputElement* GetFocusedElement() const;
        void SetFocusedElement(XUI::UI::Controls::InputElement* value);

        void SetFocusedElement(
            XUI::UI::Controls::InputElement* element,
            Interfaces::NavigationMethod method,
            Interfaces::InputModifiers modifiers);

        void ProcessRawEvent(Interfaces::RawInputEventArgs& e);

        void SetOwner(std::shared_ptr<Interfaces::UIControls::IInputRoot> const& owner);
        void OnKeyDown(void* sender, Interfaces::KeyEventArgs& e);

        static KeyboardNavigationMode GetDirectionalNavigation(XUI::UI::Controls::InputElement* element);
        static void SetDirectionalNavigation(XUI::UI::Controls::InputElement* element, KeyboardNavigationMode value);

        static KeyboardNavigationMode GetTabNavigation(XUI::UI::Controls::InputElement* element);
        static void SetTabNavigation(XUI::UI::Controls::InputElement* element, KeyboardNavigationMode value);
    };

    struct DirectionalNavigation
    {
        /// <summary>
        /// Gets the next control in the specified navigation direction.
        /// </summary>
        /// <param name="element">The element.</param>
        /// <param name="direction">The navigation direction.</param>
        /// <returns>
        /// The next element in the specified direction, or nullptr if <paramref name="element"/>
        /// was the last in the requested direction.
        /// </returns>
        static XUI::UI::Controls::InputElement* GetNext(
            XUI::UI::Controls::InputElement* element,
            UI::Presenters::NavigationDirection direction);

    private:
        /// <summary>
        /// Returns a value indicting whether the specified direction is forward.
        /// </summary>
        /// <param name="direction">The direction.</param>
        /// <returns>True if the direction is forward.</returns>
        static bool IsForward(UI::Presenters::NavigationDirection direction);

        /// <summary>
        /// Gets the first or last focusable descendant of the specified element.
        /// </summary>
        /// <param name="container">The element.</param>
        /// <param name="direction">The direction to search.</param>
        /// <returns>The element or nullptr if not found.##</returns>
        static XUI::UI::Controls::InputElement* GetFocusableDescendant(XUI::UI::Controls::InputElement* container, UI::Presenters::NavigationDirection direction);

        /// <summary>
        /// Gets the focusable descendants of the specified element.
        /// </summary>
        /// <param name="element">The element.</param>
        /// <returns>The element's focusable descendants.</returns>
        static XUI::UI::Controls::InputElement* GetFocusableDescendants(XUI::UI::Controls::InputElement* element, bool last);

        /// <summary>
        /// Gets the next item that should be focused in the specified container.
        /// </summary>
        /// <param name="element">The starting element/</param>
        /// <param name="container">The container.</param>
        /// <param name="direction">The direction.</param>
        /// <returns>The next element, or nullptr if the element is the last.</returns>
        static XUI::UI::Controls::InputElement* GetNextInContainer(
            XUI::UI::Controls::InputElement* element,
            XUI::UI::Controls::InputElement* container,
            UI::Presenters::NavigationDirection direction);

        /// <summary>
        /// Gets the first item that should be focused in the next container.
        /// </summary>
        /// <param name="element">The element being navigated away from.</param>
        /// <param name="container">The container.</param>
        /// <param name="direction">The direction of the search.</param>
        /// <returns>The first element, or nullptr if there are no more elements.</returns>
        static XUI::UI::Controls::InputElement* GetFirstInNextContainer(
            XUI::UI::Controls::InputElement* element,
            XUI::UI::Controls::InputElement* container,
            UI::Presenters::NavigationDirection direction);
    };

    struct TabNavigation
    {
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
        static XUI::UI::Controls::InputElement* GetNextInTabOrder(
            XUI::UI::Controls::InputElement* element,
            UI::Presenters::NavigationDirection direction,
            bool outsideElement = false);

    private:
        /// <summary>
        /// Gets the first or last focusable descendant of the specified element.
        /// </summary>
        /// <param name="container">The element.</param>
        /// <param name="direction">The direction to search.</param>
        /// <returns>The element or nullptr if not found.##</returns>
        static XUI::UI::Controls::InputElement* GetFocusableDescendant(XUI::UI::Controls::InputElement* container, UI::Presenters::NavigationDirection direction);

        /// <summary>
        /// Gets the focusable descendants of the specified element.
        /// </summary>
        /// <param name="element">The element.</param>
        /// <param name="direction">The tab direction. Must be Next or Previous.</param>
        /// <returns>The element's focusable descendants.</returns>
        static XUI::UI::Controls::InputElement* GetFocusableDescendants(XUI::UI::Controls::InputElement* element, UI::Presenters::NavigationDirection direction, bool last);

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
        static XUI::UI::Controls::InputElement* GetNextInContainer(
            XUI::UI::Controls::InputElement* element,
            XUI::UI::Controls::InputElement* container,
            UI::Presenters::NavigationDirection direction,
            bool outsideElement);

        /// <summary>
        /// Gets the first item that should be focused in the next container.
        /// </summary>
        /// <param name="element">The element being navigated away from.</param>
        /// <param name="container">The container.</param>
        /// <param name="direction">The direction of the search.</param>
        /// <returns>The first element, or nullptr if there are no more elements.</returns>
        static XUI::UI::Controls::InputElement* GetFirstInNextContainer(
            XUI::UI::Controls::InputElement* element,
            XUI::UI::Controls::InputElement* container,
            UI::Presenters::NavigationDirection direction);

        //static (bool handled, XUI::UI::Controls::InputElement* next) GetCustomNext(XUI::UI::Controls::InputElement* element, UI::Presenters::NavigationDirection direction)
        //{
        //    if (element is ICustomKeyboardNavigation custom)
        //    {
        //        return custom.GetNext(element, direction);
        //    }
        //
        //    return (false, nullptr);
        //}
    };
}
#endif // !__XUI_KEYBOARD_DEVICE_H__