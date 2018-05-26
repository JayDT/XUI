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

#ifndef __XUI_MOUSE_DEVICE_H__
#define __XUI_MOUSE_DEVICE_H__

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

namespace XUI::Core::Input
{
    /// <summary>
    /// Represents a mouse device.
    /// </summary>
    class TC_XUI_API MouseDevice : public Interfaces::IMouseDevice
    {
        int _clickCount;
        Core::Media::Rect _lastClickRect;
        uint64 _lastClickTime;

        std::weak_ptr<UI::Controls::InputElement> _captured;
        Core::Media::Point _position = Core::Media::Point(0, 0);

        rxcpp::subscription _inputmgr_subcribe;
    public:
        /// <summary>
        /// Intializes a new instance of <see cref="MouseDevice"/>.
        /// </summary>
        MouseDevice();
        virtual ~MouseDevice();

        /// <summary>
        /// Gets the control that is currently capturing by the mouse, if any.
        /// </summary>
        /// <remarks>
        /// When an element captures the mouse, it recieves mouse input whether the cursor is 
        /// within the control's bounds or not. To set the mouse capture, call the 
        /// <see cref="Capture"/> method.
        /// </remarks>
        PROPERTY_(UI::Controls::InputElement*, Captured);
        UI::Controls::InputElement* GetCaptured() const override { return _captured.lock().get(); }

        /// <summary>
        /// Gets the mouse position, in screen coordinates.
        /// </summary>
        PROPERTY_G(_position, Position);


        /// <summary>
        /// Captures mouse input to the specified control.
        /// </summary>
        /// <param name="control">The control.</param>
        /// <remarks>
        /// When an element captures the mouse, it recieves mouse input whether the cursor is 
        /// within the control's bounds or not. The current mouse capture control is exposed
        /// by the <see cref="Captured"/> property.
        /// </remarks>
        virtual void Capture(UI::Controls::InputElement* control);

        /// <summary>
        /// Gets the mouse position relative to a control.
        /// </summary>
        /// <param name="relativeTo">The control.</param>
        /// <returns>The mouse position in the control's coordinates.</returns>
        Core::Media::Point GetRelativePosition(UI::Controls::Visual* relativeTo) override;

        static Interfaces::InputModifiers Modifiers;

    private:
        void ProcessRawEvent(Interfaces::RawMouseEventArgs& e);

        void LeaveWindow(Interfaces::IMouseDevice* device, Interfaces::UIControls::IInputRoot* root);
        bool MouseDown(Interfaces::IMouseDevice* device, uint64 timestamp, Interfaces::UIControls::IInputRoot* root, Core::Media::Point const& p, Interfaces::MouseButton button, Interfaces::InputModifiers inputModifiers);
        bool MouseMove(Interfaces::IMouseDevice* device, Interfaces::UIControls::IInputRoot* root, Core::Media::Point const& p, Interfaces::InputModifiers inputModifiers);
        bool MouseUp(Interfaces::IMouseDevice* device, Interfaces::UIControls::IInputRoot* root, Core::Media::Point const& p, Interfaces::MouseButton button, Interfaces::InputModifiers inputModifiers);
        bool MouseWheel(Interfaces::IMouseDevice* device, Interfaces::UIControls::IInputRoot* root, Core::Media::Point const& p, Core::Media::Vector const& delta, Interfaces::InputModifiers inputModifiers);
        UI::Controls::InputElement* GetSource(UI::Controls::Visual* hit);
        UI::Controls::InputElement* HitTest(UI::Controls::Visual* root, Core::Media::Point const& p);
        void ClearPointerOver(Interfaces::IPointerDevice* device, Interfaces::UIControls::IInputRoot* root);
        UI::Controls::InputElement* SetPointerOver(Interfaces::IPointerDevice* device, Interfaces::UIControls::IInputRoot* root, Core::Media::Point const& p);
        void SetPointerOver(Interfaces::IPointerDevice* device, Interfaces::UIControls::IInputRoot* root, UI::Controls::InputElement* element);
    };
}

#endif // !__XUI_MOUSE_DEVICE_H__