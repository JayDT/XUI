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

#ifndef __XUI_STACKPANEL_H__
#define __XUI_STACKPANEL_H__

#include "UI/Panel.h"

namespace XUI::UI
{
    namespace Presenters
    {
        enum class ItemVirtualizationMode : uint8;
        enum class NavigationDirection : uint8;
    }

    /// <summary>
    /// A panel which lays out its children horizontally or vertically.
    /// </summary>
    class TC_XUI_API StackPanel : public Panel, public Interfaces::UIControls::INavigableContainer
    {
        /// <summary>
        /// Defines the <see cref="Gap"/> property.
        /// </summary>
    public:
        static Core::Dependency::RefDependencyProperty GapProperty;

        /// <summary>
        /// Defines the <see cref="Orientation"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty OrientationProperty;

        /// <summary>
        /// Initializes static members of the <see cref="StackPanel"/> class.
        /// </summary>
        static void StaticClassInitializer();
        StackPanel() {}
        virtual ~StackPanel() {}

        /// <summary>
        /// Gets or sets the size of the gap to place between child controls.
        /// </summary>
        PROPERTY_(double , Gap);
        double GetGap();
        void SetGap(double value);

        /// <summary>
        /// Gets or sets the orientation in which child controls will be layed out.
        /// </summary>
        PROPERTY_(UI::Orientation  , Orientation);
        UI::Orientation GetOrientation();
        void SetOrientation(UI::Orientation value);

        /// <summary>
        /// Gets the next control in the specified direction.
        /// </summary>
        /// <param name="direction">The movement direction.</param>
        /// <param name="from">The control from which movement begins.</param>
        /// <returns>The control.</returns>
        virtual Controls::InputElement *GetControl(Presenters::NavigationDirection direction, Controls::InputElement *from) override;

    protected:
        /// <summary>
        /// Gets the next control in the specified direction.
        /// </summary>
        /// <param name="direction">The movement direction.</param>
        /// <param name="from">The control from which movement begins.</param>
        /// <returns>The control.</returns>
        virtual Controls::InputElement *GetControlInDirection(Presenters::NavigationDirection direction, Controls::Control *from);

        /// <summary>
        /// Measures the control.
        /// </summary>
        /// <param name="availableSize">The available size.</param>
        /// <returns>The desired size of the control.</returns>
        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;

        /// <summary>
        /// Arranges the control's children.
        /// </summary>
        /// <param name="finalSize">The size allocated to the control.</param>
        /// <returns>The space taken.</returns>
        Core::Media::Size ArrangeOverride(Core::Media::Size const& finalSize) override;

    public:
        virtual void ArrangeChild(Controls::Control *child, Core::Media::Rect const& rect, Core::Media::Size const& panelSize, UI::Orientation orientation);
    };
}


#endif	//#ifndef __XUI_STACKPANEL_H__
