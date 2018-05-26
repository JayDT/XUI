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

#include "Size_t.h"
#include "Thickness_t.h"
#include "MediaTypes.h"

#include "standard/Errors.h"

using namespace XUI::Core::Media;

/// <summary>
/// A size representing infinity.
/// </summary>
const Size XUI::Core::Media::Size::Infinity(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());

/// <summary>
/// A size representing zero
/// </summary>
const Size XUI::Core::Media::Size::Empty(0, 0);

/// <summary>
/// Parses a <see cref="Size"/> string.
/// </summary>
/// <param name="s">The string.</param>
/// <param name="culture">The current culture.</param>
/// <returns>The <see cref="Size"/>.</returns>


/// <summary>
/// Initializes a new instance of the <see cref="Size"/> structure.
/// </summary>
/// <param name="width">The width.</param>
/// <param name="height">The height.</param>

XUI::Core::Media::Size::Size(double width, double height)
{
    Width = width;
    Height = height;
}

Size XUI::Core::Media::Size::Parse(std::string const & s)
{
    auto parts = System::String(std::move(s)).Trim().Split(' ');
    if (parts.size() == 2)
    {
        return Size(std::strtod(parts[0].c_str(), nullptr), std::strtod(parts[1].c_str(), nullptr));
    }
    else
    {
        throw System::FormatException("Invalid Size.");
    }
}

/// <summary>
/// Constrains the size.
/// </summary>
/// <param name="constraint">The size to constrain to.</param>
/// <returns>The constrained size.</returns>

Size XUI::Core::Media::Size::Constrain(Size const & constraint) const
{
    return Size(
        std::min(Width, constraint.Width),
        std::min(Height, constraint.Height));
}


/// <summary>
/// Adds a Thickness to a Size.
/// </summary>
/// <param name="size">The size.</param>
/// <param name="thickness">The thickness.</param>
/// <returns>The equality.</returns>
Size XUI::Core::Media::Size::operator +(Thickness const& thickness)
{
    return Size(
        Width + thickness.Left + thickness.Right,
        Height + thickness.Top + thickness.Bottom);
}

/// <summary>
/// Subtracts a Thickness from a Size.
/// </summary>
/// <param name="size">The size.</param>
/// <param name="thickness">The thickness.</param>
/// <returns>The equality.</returns>
Size XUI::Core::Media::Size::operator -(Thickness const& thickness)
{
    return Size(
        Width - (thickness.Left + thickness.Right),
        Height - (thickness.Top + thickness.Bottom));
}

/// <summary>
/// Deflates the size by a <see cref="Thickness"/>.
/// </summary>
/// <param name="thickness">The thickness.</param>
/// <returns>The deflated size.</returns>
/// <remarks>The deflated size cannot be less than 0.</remarks>

Size XUI::Core::Media::Size::Deflate(Thickness const & thickness) const
{
    return Size(
        std::max(0.0, Width - thickness.Left - thickness.Right),
        std::max(0.0, Height - thickness.Top - thickness.Bottom));
}

/// <summary>
/// Inflates the size by a <see cref="Thickness"/>.
/// </summary>
/// <param name="thickness">The thickness.</param>
/// <returns>The inflated size.</returns>

Size XUI::Core::Media::Size::Inflate(Thickness const & thickness) const
{
    return Size(
        Width + thickness.Left + thickness.Right,
        Height + thickness.Top + thickness.Bottom);
}

/// <summary>
/// Returns a new <see cref="Size"/> with the same height and the specified width.
/// </summary>
/// <param name="width">The width.</param>
/// <returns>The new <see cref="Size"/>.</returns>

Size XUI::Core::Media::Size::WithWidth(double width)
{
    return Size(width, Height);
}

/// <summary>
/// Returns the string representation of the size.
/// </summary>
/// <returns>The string representation of the size.</returns>


/// <summary>
/// Returns a new <see cref="Size"/> with the same width and the specified height.
/// </summary>
/// <param name="height">The height.</param>
/// <returns>The new <see cref="Size"/>.</returns>

Size XUI::Core::Media::Size::WithHeight(double height)
{
    return Size(Width, height);
}

std::string XUI::Core::Media::Size::ToString() const
{
    return System::String::format("%f, %f", Width, Height);
}

XUI::Core::Media::Vector XUI::Core::Media::Size::ToVector() const
{
    return Core::Media::Vector(Width, Height);
}

XUI::Core::Media::Vector XUI::Core::Media::Size::operator /(XUI::Core::Media::Size const& right)
{
    return Vector(Width / right.Width, Height / right.Height);
}

XUI::Core::Media::Size XUI::Core::Media::Size::operator *(XUI::Core::Media::Vector const& scale)
{
    return Size(Width * scale.x, Height * scale.y);
}

XUI::Core::Media::Size XUI::Core::Media::Size::operator /(XUI::Core::Media::Vector const& scale)
{
    return Size(Width / scale.x, Height / scale.y);
}

XUI::Core::Media::Size XUI::Core::Media::Size::CalculateSize(Core::Media::Stretch stretch, Size destinationSize, Size sourceSize)
{
    return sourceSize * Core::Media::Vector::CalculateScaling(stretch, destinationSize, sourceSize);
}