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

#ifndef __XUI_TOOLTIP_H__
#define __XUI_TOOLTIP_H__

#include "UI/Controls/ContentControl.h"
#include "UI/Detail/Popup.h"
#include "UI/Detail/PopupRoot.h"

namespace XUI::UI
{
    /// <summary>
    /// A control which pops up a hint when a control is hovered.
    /// </summary>
    /// <remarks>
    /// You will probably not want to create a <see cref="ToolTip"/> control directly: if added to
    /// the tree it will act as a simple <see cref="ContentControl"/> styled to look like a tooltip.
    /// assigning the content that you want displayed.
    /// </remarks>
    class ToolTip : public Controls::ContentControl
    {
    public:

        /// <summary>
        /// Defines the ToolTip.IsOpen attached property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty IsOpenProperty;

        /// <summary>
        /// Defines the ToolTip.Placement property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty PlacementProperty;

        /// <summary>
        /// Defines the ToolTip.HorizontalOffset property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty HorizontalOffsetProperty;

        /// <summary>
        /// Defines the ToolTip.VerticalOffset property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty VerticalOffsetProperty;

        /// <summary>
        /// Defines the ToolTip.ShowDelay property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty ShowDelayProperty;

    private:

        std::shared_ptr<PopupRoot> _popup;

    public:
        /// <summary>
        /// Initializes static members of the <see cref="ToolTip"/> class.
        /// </summary>
        static void StaticClassInitializer();
        virtual ~ToolTip();

        PROPERTY_(bool, IsOpen);
        /// <summary>
        /// Gets the value of the ToolTip.IsOpen attached property.
        /// </summary>
        /// <param name="element">The control to get the property from.</param>
        /// <returns>
        /// A value indicating whether the tool tip is visible.
        /// </returns>
        bool GetIsOpen();

        /// <summary>
        /// Sets the value of the ToolTip.IsOpen attached property.
        /// </summary>
        /// <param name="element">The control to get the property from.</param>
        /// <param name="value">A value indicating whether the tool tip is visible.</param>
        void SetIsOpen(bool value);

        PROPERTY_(UI::PlacementMode, Placement);
        /// <summary>
        /// Gets the value of the ToolTip.Placement attached property.
        /// </summary>
        /// <param name="element">The control to get the property from.</param>
        /// <returns>
        /// A value indicating how the tool tip is positioned.
        /// </returns>
        UI::PlacementMode GetPlacement();

        /// <summary>
        /// Sets the value of the ToolTip.Placement attached property.
        /// </summary>
        /// <param name="element">The control to get the property from.</param>
        /// <param name="value">A value indicating how the tool tip is positioned.</param>
        void SetPlacement(UI::PlacementMode value);

        PROPERTY_(double, HorizontalOffset);
        /// <summary>
        /// Gets the value of the ToolTip.HorizontalOffset attached property.
        /// </summary>
        /// <param name="element">The control to get the property from.</param>
        /// <returns>
        /// A value indicating how the tool tip is positioned.
        /// </returns>
        double GetHorizontalOffset();

        /// <summary>
        /// Sets the value of the ToolTip.HorizontalOffset attached property.
        /// </summary>
        /// <param name="element">The control to get the property from.</param>
        /// <param name="value">A value indicating how the tool tip is positioned.</param>
        void SetHorizontalOffset(double value);

        PROPERTY_(double, VerticalOffset);
        /// <summary>
        /// Gets the value of the ToolTip.VerticalOffset attached property.
        /// </summary>
        /// <param name="element">The control to get the property from.</param>
        /// <returns>
        /// A value indicating how the tool tip is positioned.
        /// </returns>
        double GetVerticalOffset();

        /// <summary>
        /// Sets the value of the ToolTip.VerticalOffset attached property.
        /// </summary>
        /// <param name="element">The control to get the property from.</param>
        /// <param name="value">A value indicating how the tool tip is positioned.</param>
        void SetVerticalOffset(double value);

        PROPERTY_(int, ShowDelay);
        /// <summary>
        /// Gets the value of the ToolTip.ShowDelay attached property.
        /// </summary>
        /// <param name="element">The control to get the property from.</param>
        /// <returns>
        /// A value indicating the time, in milliseconds, before a tool tip opens.
        /// </returns>
        int GetShowDelay();

        /// <summary>
        /// Sets the value of the ToolTip.ShowDelay attached property.
        /// </summary>
        /// <param name="element">The control to get the property from.</param>
        /// <param name="value">A value indicating the time, in milliseconds, before a tool tip opens.</param>
        void SetShowDelay(int value);

    protected:

        void OnPointerLeave(Interfaces::PointerEventArgs& e) override;

    private:
        void IsOpenChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);

        void Open(Control *control);

        void Close();
    };
}

#endif	//#ifndef __XUI_TOOLTIP_H__
