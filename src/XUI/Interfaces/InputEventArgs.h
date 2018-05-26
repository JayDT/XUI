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

#ifndef _XUI_INPUT_EVENT_ARGS_H__
#define _XUI_INPUT_EVENT_ARGS_H__

#include "Core/Media/MediaTypes.h"
#include "Core/Observer/RoutedEvent.h"
#include "Interfaces/InputKeys.h"
#include "Interfaces/IInputDevices.h"
#include "Interfaces/UIControls/IInputRoot.h"
#include "standard/events.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/Controls/Visual.h"))
MetaAssemblyEnd

#define GENERIC_SAFE_CAST(_type, _ptr) static_cast<_type>((System::Reflection::Object*)_ptr)
#define GENERIC_SAFE_AS(_type, _ptr) ((System::Reflection::Object*)_ptr)->Static_As<_type>()

namespace XUI::UI
{
    class DataGridColumn;

    namespace Controls
    {
        class Control;
    }
}

namespace XUI::Interfaces
{
    enum class RawMouseEventType
    {
        LeaveWindow,
        LeftButtonDown,
        LeftButtonUp,
        RightButtonDown,
        RightButtonUp,
        MiddleButtonDown,
        MiddleButtonUp,
        Move,
        Wheel,
        NonClientLeftButtonDown,
    };

    struct CancelEventArgs : System::Events::EventArg
    {
        CancelEventArgs() {}
        CancelEventArgs(bool cancel) : Cancel(cancel) {}

        bool Cancel = false;
    };

    struct DataGridCellClipboardEventArgs : System::Events::EventArg
    {
        DataGridCellClipboardEventArgs() {}

        System::Reflection::Variant Content;
        System::Reflection::Variant Item;
        XUI::UI::DataGridColumn* Column;
    };

    struct ItemContainerInfo
    {
        ItemContainerInfo()
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="ItemContainerInfo"/> class.
        /// </summary>
        /// <param name="container">The container control.</param>
        /// <param name="item">The item that the container represents.</param>
        /// <param name="index">
        /// The index of the item in the <see cref="ItemsControl.Items"/> collection.
        /// </param>
        ItemContainerInfo(std::shared_ptr<UI::Controls::Control> const& container, System::Reflection::Variant const& item, int index)
        {
            ContainerControl = container;
            Item = item;
            Index = index;
        }

        /// <summary>
        /// Gets the container control.
        /// </summary>
        /// <remarks>
        /// This will be null if <see cref="Item"/> is null.
        /// </remarks>
        std::shared_ptr<UI::Controls::Control> ContainerControl;

        /// <summary>
        /// Gets the item that the container represents.
        /// </summary>
        System::Reflection::Variant Item;

        /// <summary>
        /// Gets the index of the item in the <see cref="ItemsControl.Items"/> collection.
        /// </summary>
        int Index = -1;

        bool operator==(ItemContainerInfo const& rhs) const
        {
            return ContainerControl == rhs.ContainerControl && Item == rhs.Item && Index == rhs.Index;
        }
    };

    /// <summary>
    /// Provides details for the <see cref="IItemContainerGenerator.Materialized"/>
    /// and <see cref="IItemContainerGenerator.Dematerialized"/> events.
    /// </summary>
    struct ItemContainerEventArgs : System::Events::EventArg
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="ItemContainerEventArgs"/> class.
        /// </summary>
        /// <param name="startingIndex">The index of the first container in the source items.</param>
        /// <param name="containers">The containers.</param>
        /// <remarks>
        /// TODO: Do we really need to pass in StartingIndex here? The ItemContainerInfo objects
        /// have an index, and what happens if the contains passed in aren't sequential?
        /// </remarks>
        ItemContainerEventArgs(
            int startingIndex,
            System::Collection::Generic::IContainer* container)
        {
            StartingIndex = startingIndex;
            Container = container;
        }

        /// <summary>
        /// Gets the containers.
        /// </summary>
        System::Collection::Generic::IContainer* Container = nullptr;

            /// <summary>
            /// Gets the index of the first container in the source items.
            /// </summary>
        int StartingIndex = -1;
    };

    /// <summary>
    /// A raw input event.
    /// </summary>
    /// <remarks>
    /// Raw input events are sent from the windowing subsystem to the <see cref="InputManager"/>
    /// for processing: this gives an application the opportunity to pre-process the event. After
    /// pre-processing they are consumed by the relevant <see cref="Device"/> and turned into
    /// standard Avalonia events.
    /// </remarks>
    struct RawInputEventArgs : System::Events::EventArg
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="RawInputEventArgs"/> class.
        /// </summary>
        /// <param name="device">The associated device.</param>
        /// <param name="timestamp">The event timestamp.</param>
        RawInputEventArgs(IInputDevice* device, uint64 timestamp)
        {
            System::ThrowIfFailed<System::ArgumentNullException>(device != nullptr);

            Device = device;
            Timestamp = timestamp;
        }

        virtual ~RawInputEventArgs() {}

        /// <summary>
        /// Gets the associated device.
        /// </summary>
        IInputDevice* Device = nullptr;

        /// <summary>
        /// Gets or sets a value indicating whether the event was handled.
        /// </summary>
        /// <remarks>
        /// If an event is not marked handled after processing via the
        /// <see cref="InputManager"/>, then it will be passed on to the underlying OS for
        /// handling.
        /// </remarks>
        bool Handled = false;

        /// <summary>
        /// Gets the timestamp associated with the event.
        /// </summary>
        uint64 Timestamp = 0;
    };


    /// <summary>
    /// A raw mouse event.
    /// </summary>
    struct RawMouseEventArgs : RawInputEventArgs
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="RawMouseEventArgs"/> class.
        /// </summary>
        /// <param name="device">The associated device.</param>
        /// <param name="timestamp">The event timestamp.</param>
        /// <param name="root">The root from which the event originates.</param>
        /// <param name="type">The type of the event.</param>
        /// <param name="position">The mouse position, in client DIPs.</param>
        /// <param name="inputModifiers">The input modifiers.</param>
        RawMouseEventArgs(
            IInputDevice* device,
            uint64 timestamp,
            Interfaces::UIControls::IInputRoot* root,
            RawMouseEventType type,
            Core::Media::Point const& position,
            Interfaces::InputModifiers inputModifiers)
            : RawInputEventArgs(device, timestamp)
        {
            System::ThrowIfFailed<System::ArgumentNullException>(device != nullptr);
            System::ThrowIfFailed<System::ArgumentNullException>(root != nullptr);

            Root = root;
            Position = position;
            Type = type;
            InputModifiers = inputModifiers;
        }

        virtual ~RawMouseEventArgs() {}

        /// <summary>
        /// Gets the root from which the event originates.
        /// </summary>
        Interfaces::UIControls::IInputRoot* Root = nullptr;

            /// <summary>
            /// Gets the mouse position, in client DIPs.
            /// </summary>
        Core::Media::Point Position = Core::Media::Point(0, 0);

            /// <summary>
            /// Gets the type of the event.
            /// </summary>
        RawMouseEventType Type = RawMouseEventType::LeaveWindow;

            /// <summary>
            /// Gets the input modifiers.
            /// </summary>
        InputModifiers InputModifiers = InputModifiers::None;

        Core::Media::Vector Delta = Core::Media::Vector(0, 0);
    };

    struct RawMouseWheelEventArgs : RawMouseEventArgs
    {
        RawMouseWheelEventArgs(
            IInputDevice* device,
            uint64 timestamp,
            Interfaces::UIControls::IInputRoot* root,
            RawMouseEventType type,
            Core::Media::Point const& position,
            Interfaces::InputModifiers inputModifiers)
            : RawMouseEventArgs(device, timestamp, root, type, position, InputModifiers)
        { }

        virtual ~RawMouseWheelEventArgs() {}

        Core::Media::Vector Delta = Core::Media::Vector(0, 0);
    };

    enum RawKeyEventType
    {
        KeyDown,
        KeyUp
    };

    struct RawKeyEventArgs : RawInputEventArgs
    {
        RawKeyEventArgs(
            IKeyboardDevice* device,
            uint64 timestamp,
            RawKeyEventType type,
            Key key,
            InputModifiers modifiers)
            : RawInputEventArgs(device, timestamp)
        {
            Key = key;
            Type = type;
            Modifiers = modifiers;
        }

        Key Key = Key::None;

        wchar_t Char = 0;

        InputModifiers Modifiers = InputModifiers::None;

        RawKeyEventType Type = RawKeyEventType::KeyDown;
    };

    struct RawTextInputEventArgs : RawInputEventArgs
    {
        RawTextInputEventArgs(IKeyboardDevice* device, uint64 timestamp, std::wstring const& text)
            : RawInputEventArgs(device, timestamp)
        {
            Text = text;
        }

        std::wstring Text;
    };

    /// <summary>
    /// Provides data for the <see cref="SelectingItemsControl.SelectionChanged"/> event.
    /// </summary>
    struct SelectionChangedEventArgs : Core::Observer::RoutedEventArgs
    {
        SelectionChangedEventArgs(Core::Observer::RoutedEvent* routedEvent)
            : RoutedEventArgs(routedEvent)
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="SelectionChangedEventArgs"/> class.
        /// </summary>
        /// <param name="routedEvent">The event being raised.</param>
        /// <param name="addedItems">The items added to the selection.</param>
        /// <param name="removedItems">The items removed from the selection.</param>
        SelectionChangedEventArgs(Core::Observer::RoutedEvent* routedEvent, std::vector<System::Reflection::Variant> const& addedItems, std::vector<System::Reflection::Variant> const& removedItems)
            : RoutedEventArgs(routedEvent)
            , AddedItems(addedItems)
            , RemovedItems(removedItems)
        {
        }

        /// <summary>
        /// Gets the items that were added to the selection.
        /// </summary>
        std::vector<System::Reflection::Variant> AddedItems;

        /// <summary>
        /// Gets the items that were removed from the selection.
        /// </summary>
        std::vector<System::Reflection::Variant> RemovedItems;
    };

    /// <summary>
    /// Describes how an area is painted.
    /// </summary>
    struct GotFocusEventArgs : Core::Observer::RoutedEventArgs
    {
        GotFocusEventArgs() {}

        GotFocusEventArgs(Core::Observer::RoutedEvent* routedEvent)
            : RoutedEventArgs(routedEvent)
        { }
        virtual ~GotFocusEventArgs() {}

        /// <summary>
        /// Gets or sets a value indicating how the change in focus occurred.
        /// </summary>
        NavigationMethod NavigationMethod = NavigationMethod::Unspecified;

        /// <summary>
        /// Gets or sets any input modifiers active at the time of focus.
        /// </summary>
        InputModifiers InputModifiers = InputModifiers::None;
    };

    struct KeyEventArgs : Core::Observer::RoutedEventArgs
    {
        KeyEventArgs() {}

        KeyEventArgs(Core::Observer::RoutedEvent* routedEvent)
            : RoutedEventArgs(routedEvent)
        { }
        virtual ~KeyEventArgs() {}

        IKeyboardDevice* Device = nullptr;

        Key Key = Key::None;

        InputModifiers Modifiers = InputModifiers::None;
    };

    struct TextInputEventArgs : Core::Observer::RoutedEventArgs
    {
        TextInputEventArgs() {}

        TextInputEventArgs(Core::Observer::RoutedEvent* routedEvent)
            : RoutedEventArgs(routedEvent)
        { }
        virtual ~TextInputEventArgs() {}

        IKeyboardDevice* Device = nullptr;

        std::wstring Text;
    };

    struct PointerEventArgs : Core::Observer::RoutedEventArgs
    {
        PointerEventArgs()
        { }

        PointerEventArgs(Core::Observer::RoutedEvent* routedEvent)
            : RoutedEventArgs(routedEvent)
        { }

        IPointerDevice* Device = nullptr;
        InputModifiers InputModifiers = InputModifiers::None;

        virtual ~PointerEventArgs() {}

        Core::Media::Point GetPosition(UI::Controls::Visual* relativeTo)
        {
            return Device->GetRelativePosition(relativeTo);
        }
    };

    struct TC_XUI_API PointerPressedEventArgs : PointerEventArgs
    {
        PointerPressedEventArgs();

        PointerPressedEventArgs(Core::Observer::RoutedEvent* routedEvent)
            : PointerEventArgs(routedEvent)
        { }

        virtual ~PointerPressedEventArgs() {}

        int ClickCount = 0;
        MouseButton MouseButton = MouseButton::None;
    };

    struct TC_XUI_API PointerReleasedEventArgs : PointerEventArgs
    {
        PointerReleasedEventArgs();

        PointerReleasedEventArgs(Core::Observer::RoutedEvent* routedEvent)
            : PointerEventArgs(routedEvent)
        { }

        virtual ~PointerReleasedEventArgs() {}

        MouseButton MouseButton = MouseButton::None;
    };

    struct PointerWheelEventArgs : PointerEventArgs
    {
        PointerWheelEventArgs() {}

        PointerWheelEventArgs(Core::Observer::RoutedEvent* routedEvent)
            : PointerEventArgs(routedEvent)
        { }

        virtual ~PointerWheelEventArgs() {}

        Core::Media::Vector Delta = Core::Media::Vector(0, 0);
    };

    struct VectorEventArgs : Core::Observer::RoutedEventArgs
    {
        VectorEventArgs() {}

        VectorEventArgs(Core::Observer::RoutedEvent* routedEvent)
            : RoutedEventArgs(routedEvent)
        { }

        virtual ~VectorEventArgs() {}

        Core::Media::Vector Vector = Core::Media::Vector(0, 0);
    };

    struct InputGesture
    {
        rxcpp::composite_subscription Unregisters;

        virtual bool Matches(System::Reflection::Object*, Core::Observer::RoutedEventArgs& e) = 0;
    };
}

#endif // !_XUI_INPUT_EVENT_ARGS_H__
