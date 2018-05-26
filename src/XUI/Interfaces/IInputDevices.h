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

#ifndef _XUI_INPUT_DEVICES_H__
#define _XUI_INPUT_DEVICES_H__

#include "XamlCPP/Core/NotifyPropertyChanged.h"
#include "Core/Media/MediaTypes.h"
#include <boost/uuid/uuid.hpp>
#include <rxcpp/rx.hpp>

MetaAssemblyBegin
Meta(module: ReflInclude("UI/Controls/InputElement.h"))
MetaAssemblyEnd

namespace XUI::UI::Controls
{
    class InputElement;
    class Visual;
}

namespace XUI::Interfaces
{
    struct RawInputEventArgs;

    /// <summary>
    /// Defines the method by which a focus change occurred.
    /// </summary>
    enum class NavigationMethod
    {
        /// <summary>
        /// The focus was changed by an unspecified method, e.g. calling
        /// <see cref="InputElement.Focus"/>.
        /// </summary>
        Unspecified,

        /// <summary>
        /// The focus was changed by the user tabbing between control.
        /// </summary>
        Tab,

        /// <summary>
        /// The focus was changed by the user pressing a directional navigation key.
        /// </summary>
        Directional,

        /// <summary>
        /// The focus was changed by a pointer click.
        /// </summary>
        Pointer,
    };

    enum InputModifiers
    {
        None = 0,
        Alt = 1,
        Control = 2,
        Shift = 4,
        Windows = 8,
        LeftMouseButton = 16,
        RightMouseButton = 32,
        MiddleMouseButton = 64
    };

    enum class KeyStates
    {
        None = 0,
        Down = 1,
        Toggled = 2,
    };

    enum class MouseButton
    {
        None,
        Left,
        Right,
        Middle
    };

    struct
        Meta(ReflInclude("UI/Controls/InputElement.h"))
        IInputDevice
    {
        virtual ~IInputDevice() {}
    };

    struct IKeyboardDevice : IInputDevice, XamlCPP::Core::NotifyPropertyChanged
    {
        virtual ~IKeyboardDevice() {}

        //// {65F11B84-17FB-469E-BA26-637DA76FDE39}
        static constexpr boost::uuids::uuid __uuid = { 0x65, 0xf1, 0x1b, 0x84, 0x17, 0xfb, 0x46, 0x9e, 0xba, 0x26, 0x63, 0x7d, 0xa7, 0x6f, 0xde, 0x39 };

        PROPERTY_GET_ONLY(XUI::UI::Controls::InputElement*, FocusedElement);
        virtual XUI::UI::Controls::InputElement* GetFocusedElement() const = 0;

        virtual void SetFocusedElement(
            UI::Controls::InputElement* element,
            NavigationMethod method,
            InputModifiers modifiers) = 0;
    };

    struct IPointerDevice : IInputDevice
    {
        virtual ~IPointerDevice() {}

        PROPERTY_GET_ONLY(UI::Controls::InputElement*, Captured);
        virtual UI::Controls::InputElement* GetCaptured() const = 0;

        virtual void Capture(UI::Controls::InputElement* control) = 0;

        virtual Core::Media::Point GetRelativePosition(UI::Controls::Visual* relativeTo) = 0;
    };

    /// <summary>
    /// Represents a mouse device.
    /// </summary>
    struct IMouseDevice : IPointerDevice
    {
        virtual ~IMouseDevice() {}

        //// {1BC9534B-CB30-45AF-A0C5-01247900379C}
        static constexpr boost::uuids::uuid __uuid = { 0x1b, 0xc9, 0x53, 0x4b, 0xcb, 0x30, 0x45, 0xaf, 0xa0, 0xc5, 0x1, 0x24, 0x79, 0x0, 0x37, 0x9c };

        /// <summary>
        /// Gets the mouse position, in screen coordinates.
        /// </summary>
        PROPERTY_TG(Core::Media::Point, Position)
        virtual PROPERTY_T_GET(Position, ) { return Core::Media::Point::Zero();}
    };

    struct IInputMgr
    {
        virtual ~IInputMgr() {}

        //// {F7BAF76C-9D35-4A02-B25D-F541302E8D83}
        static constexpr boost::uuids::uuid __uuid = { 0xf7, 0xba, 0xf7, 0x6c, 0x9d, 0x35, 0x4a, 0x02, 0xb2, 0x5d, 0xf5, 0x41, 0x30, 0x2e, 0x8d, 0x83 };

        typedef rxcpp::subjects::subject<RawInputEventArgs> InputObservable;

        PROPERTY_GET_ONLY(InputObservable&, PreProcess);
        virtual InputObservable& PROPERTY_GET(PreProcess)() = 0;

        PROPERTY_GET_ONLY(InputObservable&, Process);
        virtual InputObservable& PROPERTY_GET(Process)() = 0;

        PROPERTY_GET_ONLY(InputObservable&, PostProcess);
        virtual InputObservable& PROPERTY_GET(PostProcess)() = 0;

        virtual void ProcessInput(RawInputEventArgs& e) = 0;
    };
}

#endif //!_XUI_INPUT_DEVICES_H__