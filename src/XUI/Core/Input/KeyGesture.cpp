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

#include "KeyGesture.h"

using namespace XUI;

XUI::Core::Input::KeyGesture::KeyGesture(Interfaces::Key key, Interfaces::InputModifiers modifiers)
    : Key(key)
    , Modifiers(modifiers)
{
}

//TODO: Move that to external key parser
Interfaces::Key XUI::Core::Input::KeyGesture::ParseKey(std::string const& key)
{
    if (key[0] == '+')
        return Interfaces::Key::OemPlus;
    if (key[0] == '-')
        return Interfaces::Key::OemMinus;
    if (key[0] == '.')
        return Interfaces::Key::OemPeriod;
    return (Interfaces::Key)System::Reflection::Enum::Parse(typeof(Interfaces::Key), key).ToLong();
}

Interfaces::InputModifiers XUI::Core::Input::KeyGesture::ParseModifier(std::string modifier)
{
    if (modifier == "Ctrl")
        return Interfaces::InputModifiers::Control;
    return (Interfaces::InputModifiers)System::Reflection::Enum::Parse(typeof(Interfaces::InputModifiers), modifier).ToLong();
}

std::shared_ptr<XUI::Core::Input::KeyGesture> XUI::Core::Input::KeyGesture::Parse(std::string gesture)
{
    std::vector<System::String> parts;
    
    int cstart = 0;
    for (int c = 0; c <= gesture.size(); c++)
    {
        int ch = c == gesture.size() ? '\0' : gesture[c];
        if (c == gesture.size() || (ch == '+' && cstart != c))
        {
            parts.push_back(gesture.substr(cstart, c - cstart));
            cstart = c + 1;
        }
    }
    for (int c = 0; c < parts.size(); c++)
        parts[c] = parts[c].Trim();
    
    Interfaces::Key key;
    Interfaces::InputModifiers modifiers = Interfaces::InputModifiers::None;
    for (int c = 0; c < parts.size(); c++)
    {
        if (c == parts.size() - 1)
            key = ParseKey(parts[c]);
        else
            modifiers = Interfaces::InputModifiers(modifiers | uint32(ParseModifier(parts[c])));
    }

    return std::make_shared<KeyGesture>(key, modifiers);
}

std::string XUI::Core::Input::KeyGesture::ToString() const
{
    std::string str;
    for (auto flag : typeof(Interfaces::InputModifiers).GetEnum()->GetValues())
    {
        if (Modifiers & flag.ToLong())
            str += typeof(Interfaces::InputModifiers).GetEnum()->GetKey(flag.ToLong());
    }

    str += " + " + typeof(Interfaces::Key).GetEnum()->GetKey((uint32)Key);
    return str;
}

bool XUI::Core::Input::KeyGesture::Matches(System::Reflection::meta::Object *, XUI::Core::Observer::RoutedEventArgs &e)
{
    Interfaces::KeyEventArgs* _e = e.Static_As<Interfaces::KeyEventArgs>();
    if (_e)
        return _e->Key == Key && (_e->Modifiers & Modifiers) == Modifiers;
    return false;
}
