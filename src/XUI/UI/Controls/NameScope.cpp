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

#include "NameScope.h"
#include <System/rtString.h>

using namespace XUI::UI::Controls;

XUI::UI::Controls::NameScope::NameScope()
{
    //_inner.set_empty_key("");
}

XUI::UI::Controls::NameScope::~NameScope()
{
}

void XUI::UI::Controls::NameScope::Register(std::string const & name, System::Reflection::RefObject element)
{
    System::ThrowIfFailed<System::ArgumentException>(!name.empty());
    System::ThrowIfFailed<System::ArgumentException>(element != nullptr);

    System::Reflection::Object* existing = nullptr;

    auto _inner_iterator = _inner.find(name);
    if (_inner_iterator != _inner.end())
    {
        existing = _inner_iterator->second.lock().get();
        if (existing != element.get())
        {
            throw System::ArgumentException(System::String::format("Control with the name '%s' already registered.", name.c_str()).c_str());
        }
    }
    else
    {
        _inner.emplace(name, element);
        if (Registered)
        {
            XamlCPP::Interfaces::NameScopeEventArgs arg(name, element);
            Registered(this, arg);
        }
    }
}

System::Reflection::RefObject XUI::UI::Controls::NameScope::Find(const std::string &name)
{
    System::ThrowIfFailed<System::ArgumentException>(!name.empty());

    auto _inner_iterator = _inner.find(name);
    if (_inner_iterator != _inner.end())
        return _inner_iterator->second.lock();
    return nullptr;
}

void XUI::UI::Controls::NameScope::Unregister(const std::string &name)
{
    System::ThrowIfFailed<System::ArgumentException>(name != "");

    System::Reflection::RefObject element;

    auto _inner_iterator = _inner.find(name);
    if (_inner_iterator != _inner.end())
    {
        element = _inner_iterator->second.lock();
        _inner.erase(_inner_iterator);
        if (Unregistered)
        {
            XamlCPP::Interfaces::NameScopeEventArgs arg(name, element);
            Unregistered(this, arg);
        }
    }
}
