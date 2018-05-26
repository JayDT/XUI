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

#ifndef __XUI_KEY_GESTURES_H__
#define __XUI_KEY_GESTURES_H__

#include "Core/Dependencies/Models/DataProperty.h"
#include "Core/Dependencies/Models/DirectProperty.h"
#include "Core/Observer/RoutedEvent.h"

#include "standard/BasicPrimities.h"

#include "Interfaces/IBrush.h"
#include "Interfaces/ICursor.h"
#include "Interfaces/InputEventArgs.h"
#include "Interfaces/InputKeys.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/Controls/Visual.h"))
MetaAssemblyEnd

namespace XUI::UI::Controls
{
    class Visual;
}

namespace XUI::Core::Input
{
    struct KeyGesture final : Interfaces::InputGesture
    {
        bool operator==(KeyGesture const& other) const
        {
            return Key == other.Key && Modifiers == other.Modifiers;
        }

        size_t GetHashCode() const
        {
            return ((int)Key * 397) ^ (int)Modifiers;
        }

        bool operator !=(KeyGesture const& right)
        {
            return !(*this == right);
        }

        KeyGesture() {}
        KeyGesture(Interfaces::Key key, Interfaces::InputModifiers modifiers = Interfaces::InputModifiers::None);
        virtual ~KeyGesture() {}

        Interfaces::Key Key;
        Interfaces::InputModifiers Modifiers;

        static Interfaces::Key ParseKey(std::string const& key);

        static Interfaces::InputModifiers ParseModifier(std::string modifier);

        static std::shared_ptr<KeyGesture> Parse(std::string gesture);

        std::string ToString() const;

        // Inherited via InputGesture
        virtual bool Matches(System::Reflection::meta::Object *, XUI::Core::Observer::RoutedEventArgs &e);
    };
}
#endif // !__XUI_KEY_GESTURES_H__