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
#include "GridLength.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/case_conv.hpp>

#include <boost/algorithm/string.hpp>

using namespace XUI::UI;

/// <summary>
/// Parses a string to return a <see cref="GridLength"/>.
/// </summary>
/// <param name="s">The string.</param>
/// <param name="culture">The current culture.</param>
/// <returns>The <see cref="GridLength"/>.</returns>

GridLength XUI::UI::GridLength::Parse(const std::string & _s)
{
    const std::string& s = _s; // boost::to_upper_copy(_s, std::locale(""));

    if (boost::iequals(s, "Auto"))
    {
        return GetAuto();
    }
    else if (boost::ends_with(s, "*"))
    {
        auto valueString = s.length() > 1 ? System::String(s.substr(0, s.length() - 1)).Trim() : System::String();
        auto value = valueString.length() > 0 ? std::stod(valueString) : 1;
        return GridLength(value, GridUnitType::Star);
    }
    else
    {
        auto value = std::stod(s);
        return GridLength(value, GridUnitType::Pixel);
    }
}
