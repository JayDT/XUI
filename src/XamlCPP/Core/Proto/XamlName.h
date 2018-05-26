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

#pragma once

#include <System/System.h>
#include <standard/misc.h>

namespace XamlCPP::Core::Proto
{
    class TC_XAMLCPP_API XamlName : public System::EnableSharedFromThisEx<XamlName>
    {
    protected:

    public:
        std::string Namespace;
        std::string Prefix;
        std::string PropertyName;

    protected:
        XamlName(std::string const& propertyName);
        XamlName(std::string const& prefix, std::string const& propertyName);

    public:
        static bool ContainsDot(std::string const& name);
        static bool IsValidXamlName(std::string const& name);
        static bool IsValidNameStartChar(char ch);
        static bool IsValidNameChar(char ch);
        static bool IsValidQualifiedNameChar(char ch);
    };
}
