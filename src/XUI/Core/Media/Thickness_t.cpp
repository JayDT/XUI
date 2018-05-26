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

#include "Thickness_t.h"
#include "Size_t.h"
#include "Core/Media/MediaTypes.h"

using namespace XUI::Core::Media;

/// <summary>
/// Parses a <see cref="Thickness"/> string.
/// </summary>
/// <param name="s">The string.</param>
/// <param name="culture">The current culture.</param>
/// <returns>The <see cref="Thickness"/>.</returns>


/// <summary>
/// Initializes a new instance of the <see cref="Thickness"/> structure.
/// </summary>
/// <param name="uniformLength">The length that should be applied to all sides.</param>

XUI::Core::Media::Thickness::Thickness(double uniformLength)
{
    Left = Top = Right = Bottom = uniformLength;
}

/// <summary>
/// Initializes a new instance of the <see cref="Thickness"/> structure.
/// </summary>
/// <param name="horizontal">The thickness on the left and right.</param>
/// <param name="vertical">The thickness on the top and bottom.</param>

XUI::Core::Media::Thickness::Thickness(double horizontal, double vertical)
{
    Left = Right = horizontal;
    Top = Bottom = vertical;
}

/// <summary>
/// Initializes a new instance of the <see cref="Thickness"/> structure.
/// </summary>
/// <param name="left">The thickness on the left.</param>
/// <param name="top">The thickness on the top.</param>
/// <param name="right">The thickness on the right.</param>
/// <param name="bottom">The thickness on the bottom.</param>

XUI::Core::Media::Thickness::Thickness(double left, double top, double right, double bottom)
{
    Left = left;
    Top = top;
    Right = right;
    Bottom = bottom;
}

/// <summary>
/// Gets a value indicating whether all sides are set to 0.
/// </summary>

bool XUI::Core::Media::Thickness::IsEmpty() const 
{ 
    return Left == 0 && Top == 0 && Right == 0 && Bottom == 0; 
}

Thickness XUI::Core::Media::Thickness::Parse(std::string const & s)
{
    auto parts = System::String(std::move(s)).Trim().Split(',');

    switch (parts.size())
    {
        case 1:
        {
            double uniform = std::strtod(parts[0].c_str(), nullptr);
            return Thickness(uniform);
        }
        case 2:
        {
            double horizontal = std::strtod(parts[0].c_str(), nullptr);
            double vertical = std::strtod(parts[1].c_str(), nullptr);
            return Thickness(horizontal, vertical);
        }
        case 4:
        {
            double left    = std::strtod(parts[0].c_str(), nullptr);
            double top     = std::strtod(parts[1].c_str(), nullptr);
            double right   = std::strtod(parts[2].c_str(), nullptr);
            double bottom  = std::strtod(parts[3].c_str(), nullptr);
            return Thickness(left, top, right, bottom);
        }
    }

    throw System::FormatException("Invalid Thickness.");
}

/// <summary>
/// Returns the string representation of the thickness.
/// </summary>
/// <returns>The string representation of the thickness.</returns>

std::string XUI::Core::Media::Thickness::ToString() const
{
    return System::String::format("%f, %f, %f, %f", Left, Top, Right, Bottom);
}

/// <summary>
/// Deflates the rectangle by a <see cref="Thickness"/>.
/// </summary>
/// <param name="thickness">The thickness.</param>
/// <returns>The deflated rectangle.</returns>
/// <remarks>The deflated rectangle size cannot be less than 0.</remarks>

void XUI::Core::Media::Thickness::Deflate(Rect& rect)
{
    Size size(rect.width(), rect.height());
    size = size.Deflate(*this);

    rect = Rect::xywh(Core::Media::Vector(rect.x0() + Left, rect.y0() + Top), size.ToVector());
}
