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

#ifndef __XUI_GESTURES_H__
#define __XUI_GESTURES_H__

#include "Core/Dependencies/Models/DataProperty.h"
#include "Core/Dependencies/Models/DirectProperty.h"
#include "Core/Observer/RoutedEvent.h"

#include "standard/BasicPrimities.h"

#include "Interfaces/IBrush.h"
#include "Interfaces/ICursor.h"
#include "Interfaces/InputEventArgs.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/Controls/Visual.h"))
MetaAssemblyEnd

namespace XUI::UI::Controls
{
    class Visual;
}

namespace XUI::Core::Input
{
    struct Gestures
    {
        static std::shared_ptr<Observer::RoutedEvent> TappedEvent;
        static std::shared_ptr<Observer::RoutedEvent> DoubleTappedEvent;
        static std::weak_ptr<UI::Controls::Visual> s_lastPress;

        static void PointerPressed(Observer::RoutedEventArgs const& ev);
        static void PointerReleased(Observer::RoutedEventArgs const& ev);
    };
}
#endif // !__XUI_GESTURES_H__