/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 OmniGUI Platform
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


//File: XamlName.cpp

#include "XamlName.h"
#include <ctype.h>

using namespace XamlCPP::Core::Proto;

XamlCPP::Core::Proto::XamlName::XamlName(std::string const& propertyName)
    : PropertyName(propertyName)
{
}

XamlCPP::Core::Proto::XamlName::XamlName(std::string const& prefix, std::string const& propertyName)
    : Prefix(prefix)
    , PropertyName(propertyName)
{
}

bool XamlCPP::Core::Proto::XamlName::ContainsDot(std::string const& name)
{
    return name.find('.') != std::string::npos;
}

bool XamlCPP::Core::Proto::XamlName::IsValidXamlName(std::string const& name)
{
    if (name.empty() || !IsValidNameStartChar(name[0]))
        return false;

    for (int index = 1; index < name.size(); ++index)
    {
        if (!IsValidNameChar(name[index]))
        {
            return false;
        }
    }

    return true;
}

bool XamlCPP::Core::Proto::XamlName::IsValidNameStartChar(char ch)
{
    if (!System::Char::IsLetter(ch))
    {
        return ch == '_';
    }
    return true;
}

bool XamlCPP::Core::Proto::XamlName::IsValidNameChar(char ch)
{
    return IsValidNameStartChar(ch) || isdigit(ch);
}

bool XamlCPP::Core::Proto::XamlName::IsValidQualifiedNameChar(char ch)
{
    return ch == '.' || IsValidNameChar(ch);
}
