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

#include "StdAfx.h"
#include "MediaTypes.h"
#include "Core/Media/Colors.h"

#include <boost/algorithm/string.hpp>

/// <summary>
/// Parses a color string.
/// </summary>
/// <param name="s">The color string.</param>
/// <returns>The <see cref="Color"/>.</returns>

XUI::Core::Media::Color XUI::Core::Media::Color::Parse(std::string const & s)
{
    if (s[0] == '#')
    {
        auto or = 0u;
        uint32 value = 0u;
        if (s.size() == 4)
        {
            auto a = 0xFF;
            auto r = uint32(255 * (float(std::strtoul(s.substr(1, 1).c_str(), nullptr, 16)) / float(0xF)));
            auto g = uint32(255 * (float(std::strtoul(s.substr(2, 1).c_str(), nullptr, 16)) / float(0xF)));
            auto b = uint32(255 * (float(std::strtoul(s.substr(3, 1).c_str(), nullptr, 16)) / float(0xF)));
            value = a << 24 | r << 16 | g << 8 | b;
        }
        else if (s.size() == 5)
        {
            auto a = uint32(255 * (float(std::strtoul(s.substr(1, 1).c_str(), nullptr, 16)) / float(0xF)));
            auto r = uint32(255 * (float(std::strtoul(s.substr(2, 1).c_str(), nullptr, 16)) / float(0xF)));
            auto g = uint32(255 * (float(std::strtoul(s.substr(3, 1).c_str(), nullptr, 16)) / float(0xF)));
            auto b = uint32(255 * (float(std::strtoul(s.substr(4, 1).c_str(), nullptr, 16)) / float(0xF)));
            value = a << 24 | r << 16 | g << 8 | b;
        }
        else if (s.size() == 7)
        {
            or = 0xff000000;
            value = std::strtoul(s.substr(1).c_str(), nullptr, 16);
        }
        else if (s.size() != 9)
        {
            throw System::FormatException("Invalid color string: ''.");
        }
        else
        {
            value = std::strtoul(s.substr(1).c_str(), nullptr, 16);
        }

        return Color(value | or);
    }
    else
    {
        //auto name = System::String(s).ToLower();
        static auto member = typeof(Colors).GetProperties();

        if (member)
        {
            for (auto p : *member)
            {
                //if (System::String(p->GetName()).ToLower() == name)
                if (boost::iequals(s, p->GetName()))
                    return p->GetValue(nullptr).GetValue<Color>();
            }
        }
    }

    throw System::FormatException("Invalid color string: ''.");
}
