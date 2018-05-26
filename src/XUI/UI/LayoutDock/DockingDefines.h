/*
* Copyright (C) 2017-2018 Tauri JayD <https://www.>
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

#ifndef __XUI_UI_LAYOUT_DOCK_DEFINES_H__
#define __XUI_UI_LAYOUT_DOCK_DEFINES_H__

#include "standard/enum.h"

namespace XUI::UI
{
    enum class ChildrenTreeChange : uint8
    {
        None,

        /// <summary>
        /// Direct insert/remove operation has been perfomed to the group
        /// </summary>
        DirectChildrenChanged,

        /// <summary>
        /// An element below in the hierarchy as been added/removed
        /// </summary>
        TreeChanged
    };

    enum class AnchorSide
    {
        Left,

        Top,

        Right,

        Bottom
    };

    enum class AnchorableShowStrategy : byte
    {
        Most = 0x0001,
        Left = 0x0002,
        Right = 0x0004,
        Top = 0x0010,
        Bottom = 0x0020,
    };

    ENABLE_ENUM_CLASS_FLAG(AnchorableShowStrategy)

    enum class DropAreaType
    {
        DockingManager,

        DocumentPane,

        DocumentPaneGroup,

        AnchorablePane,
    };

    enum class DropTargetType
    {
        DockingManagerDockLeft,
        DockingManagerDockTop,
        DockingManagerDockRight,
        DockingManagerDockBottom,

        DocumentPaneDockLeft,
        DocumentPaneDockTop,
        DocumentPaneDockRight,
        DocumentPaneDockBottom,
        DocumentPaneDockInside,

        DocumentPaneGroupDockInside,

        AnchorablePaneDockLeft,
        AnchorablePaneDockTop,
        AnchorablePaneDockRight,
        AnchorablePaneDockBottom,
        AnchorablePaneDockInside,

        DocumentPaneDockAsAnchorableLeft,
        DocumentPaneDockAsAnchorableTop,
        DocumentPaneDockAsAnchorableRight,
        DocumentPaneDockAsAnchorableBottom,
    };

    enum class OverlayWindowDropTargetType
    {
        DockingManagerDockLeft,
        DockingManagerDockTop,
        DockingManagerDockRight,
        DockingManagerDockBottom,

        DocumentPaneDockLeft,
        DocumentPaneDockTop,
        DocumentPaneDockRight,
        DocumentPaneDockBottom,
        DocumentPaneDockInside,

        AnchorablePaneDockLeft,
        AnchorablePaneDockTop,
        AnchorablePaneDockRight,
        AnchorablePaneDockBottom,
        AnchorablePaneDockInside,

    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_DEFINES_H__