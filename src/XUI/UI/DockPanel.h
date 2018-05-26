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

#ifndef __XUI_DOCKPANEL_H__
#define __XUI_DOCKPANEL_H__

#include "UI/Panel.h"

namespace XUI::UI
{
    /// <summary>
    /// Defines the available docking modes for a control in a <see cref="DockPanel"/>.
    /// </summary>
    enum class Dock : uint8
    {
        Left = 0,
        Bottom,
        Right,
        Top
    };

    /// <summary>
    /// A panel which arranges its children at the top, bottom, left, right or center.
    /// </summary>
    class TC_XUI_API DockPanel : public Panel
    {
        /// <summary>
        /// Defines the Dock attached property.
        /// </summary>
    public:
        static Core::Dependency::RefDependencyProperty DockProperty;

        /// <summary>
        /// Defines the <see cref="LastChildFill"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty LastChildFillProperty;

        /// <summary>
        /// Initializes static members of the <see cref="DockPanel"/> class.
        /// </summary>

        static void StaticConstructor();

        /// <summary>
        /// Gets the value of the Dock attached property on the specified control.
        /// </summary>
        /// <param name="control">The control.</param>
        /// <returns>The Dock attached property.</returns>
        static Dock GetDock(Controls::Visual* control);

        /// <summary>
        /// Sets the value of the Dock attached property on the specified control.
        /// </summary>
        /// <param name="control">The control.</param>
        /// <param name="value">The value of the Dock property.</param>
        static void SetDock(Controls::Visual* control, Dock value);

        /// <summary>
        /// Gets or sets a value which indicates whether the last child of the 
        /// <see cref="DockPanel"/> fills the remaining space in the panel.
        /// </summary>
        PROPERTY_(bool, LastChildFill);
        bool GetLastChildFill() const;
        void SetLastChildFill(const bool &value);

    protected:

        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;
        Core::Media::Size ArrangeOverride(Core::Media::Size const& arrangeSize) override;
    };
}


#endif	//#ifndef __XUI_DOCKPANEL_H__
