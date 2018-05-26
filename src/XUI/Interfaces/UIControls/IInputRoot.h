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

#ifndef __XUI_IINPUT_ROOT_H__
#define __XUI_IINPUT_ROOT_H__

#include "Core/Media/MediaTypes.h"
#include "standard/events.h"

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
    /// <summary>
    /// Interface for Avalonia controls.
    /// </summary>
    struct Meta(Enable) IInputRoot
    {
        virtual UI::Controls::Visual* ToVisualBase() const = 0;
        virtual Core::Media::Point PointToClient(Core::Media::Point const& point) = 0;
        virtual Core::Media::Point PointToScreen(Core::Media::Point const& point) = 0;
    };

    /// <summary>
    /// Designates a control as handling its own keyboard navigation.
    /// </summary>
    struct Meta(Enable) ICustomKeyboardNavigation
    {
        virtual UI::Controls::InputElement* GetNext(UI::Controls::InputElement* element, UI::Presenters::NavigationDirection direction, bool& handled) = 0; 
    };

    struct Meta(Enable) INavigableContainer
    {
        virtual UI::Controls::InputElement *GetControl(UI::Presenters::NavigationDirection direction, UI::Controls::InputElement *from) = 0;
    };
}

#endif // !__IDESCRIPTION_H__
