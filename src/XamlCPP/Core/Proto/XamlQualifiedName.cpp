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

#include "XamlQualifiedName.h"

bool XamlCPP::Core::Proto::XamlQualifiedName::IsNameValid(std::string const & name)
{
    if (name.empty() || !IsValidNameStartChar(name[0]))
        return false;

    for (char c : name)
    {
        if (!IsValidQualifiedNameChar(c))
            return false;
    }

    return true;
}

XamlCPP::Core::Proto::XamlQualifiedName::XamlQualifiedName(std::string const & prefix, std::string const & propertyName)
    : XamlName(prefix, propertyName)
{
}

bool XamlCPP::Core::Proto::XamlQualifiedName::TryParse(std::string const & longName, std::string& prefix, std::string& name)
{
    auto startIndex = 0;
    auto length = longName.find(':');
    prefix = "";
    name = "";
    if (length != std::string::npos)
    {
        prefix = longName.substr(startIndex, length);
        if (prefix.empty() || !IsNameValid(prefix))
            return false;
        startIndex = length + 1;
    }
    name = startIndex == 0 ? longName : longName.substr(startIndex);
    return !name.empty();
}
