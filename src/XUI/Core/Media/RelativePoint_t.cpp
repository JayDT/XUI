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

#include "RelativePoint_t.h"
#include "System/rtString.h"

#include "cpplinq/linq.hpp"

extern std::locale loc;

using namespace XUI;

/// <summary>
/// A point at the top left of the containing element.
/// </summary>
const XUI::Core::Media::RelativePoint XUI::Core::Media::RelativePoint::TopLeft(0, 0, XUI::Core::Media::RelativeUnit::Relative);

/// <summary>
/// A point at the center of the containing element.
/// </summary>
const XUI::Core::Media::RelativePoint XUI::Core::Media::RelativePoint::Center(0.5, 0.5, XUI::Core::Media::RelativeUnit::Relative);

/// <summary>
/// A point at the bottom right of the containing element.
/// </summary>
const XUI::Core::Media::RelativePoint XUI::Core::Media::RelativePoint::BottomRight(1, 1, XUI::Core::Media::RelativeUnit::Relative);

/// <summary>
/// Initializes a new instance of the <see cref="RelativePoint"/> struct.
/// </summary>
/// <param name="point">The point.</param>
/// <param name="unit">The unit.</param>

XUI::Core::Media::RelativePoint::RelativePoint(XUI::Core::Media::Point point, RelativeUnit unit)
    : _unit(unit)
{
    _point = point;
}

/// <summary>
/// Initializes a new instance of the <see cref="RelativePoint"/> struct.
/// </summary>
/// <param name="x">The X point.</param>
/// <param name="y">The Y point</param>
/// <param name="unit">The unit.</param>

XUI::Core::Media::RelativePoint::RelativePoint(double x, double y, RelativeUnit unit)
    : RelativePoint(Core::Media::Point(x, y), unit)
{
}

/// <summary>
/// Converts a <see cref="RelativePoint"/> into pixels.
/// </summary>
/// <param name="size">The size of the visual.</param>
/// <returns>The origin point in pixels.</returns>
Core::Media::Point XUI::Core::Media::RelativePoint::ToPixels(Core::Media::Size size)
{
    return _unit == RelativeUnit::Absolute ?
        _point : Core::Media::Point(_point.x * size.Width, _point.y * size.Height);
}

/// <summary>
/// Parses a <see cref="RelativePoint"/> string.
/// </summary>
/// <param name="s">The string.</param>
/// <param name="culture">The current culture.</param>
/// <returns>The parsed <see cref="RelativePoint"/>.</returns>
XUI::Core::Media::RelativePoint XUI::Core::Media::RelativePoint::Parse(std::string const & s)
{
    auto parts = cpplinq::from(System::String(s).Split(','))
        .select([](std::string const& x) { return System::String(x).Trim(); })
        .to_vector();

    if (parts.size() == 2)
    {
        RelativeUnit unit = RelativeUnit::Relative; // Absolute;
        float scale = 1.0;

        if (parts[0].find_last_of("%") != std::string::npos)
        {
            if (parts[1].find_last_of("%") == std::string::npos)
            {
                throw System::FormatException("If one coordinate is relative, both must be.");
            }

            parts[0] = parts[0].Trim('%');
            parts[1] = parts[1].Trim('%');
            unit = RelativeUnit::Relative;
            scale = 0.01;
        }

        std::stringstream ssx(parts[0].c_str());
        ssx.imbue(loc);
        double x;
        ssx >> x;

        std::stringstream ssy(parts[1].c_str());
        ssy.imbue(loc);
        double y;
        ssy >> y;

        return RelativePoint(
            x * scale,
            y * scale,
            unit);
    }
    else
    {
        throw System::FormatException("Invalid Point.");
    }
}
