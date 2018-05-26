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
#include "Core/Media/MediaTypes.h"
#include "Core/Media/Size_t.h"
#include "Core/Media/Thickness_t.h"

using namespace XUI::Core::Media;

const Rect Rect::Empty;

/// <summary>
/// Deflates the rectangle.
/// </summary>
/// <param name="thickness">The thickness.</param>
/// <returns>The deflated rectangle.</returns>
/// <remarks>The deflated rectangle size cannot be less than 0.</remarks>
Rect XUI::Core::Media::Rect::Deflate(double thickness)
{
    return Deflate(Thickness(thickness / 2));
}

/// <summary>
/// Deflates the rectangle by a <see cref="Thickness"/>.
/// </summary>
/// <param name="thickness">The thickness.</param>
/// <returns>The deflated rectangle.</returns>
/// <remarks>The deflated rectangle size cannot be less than 0.</remarks>
Rect XUI::Core::Media::Rect::Deflate(Thickness const & thickness)
{
    return Rect(Point(x0() + thickness.Left, y0() + thickness.Top), Size(width(), height()).Deflate(thickness).ToVector());
}

Rect XUI::Core::Media::Rect::Inflate(Thickness const& thickness)
{
    return Rect(Point(x0() - thickness.Left, y0() - thickness.Top), Size(width(), height()).Inflate(thickness).ToVector());
}

/// <summary>
/// Returns the axis-aligned bounding box of a transformed rectangle.
/// </summary>
/// <param name="matrix">The transform.</param>
/// <returns>The bounding box</returns>
Rect XUI::Core::Media::Rect::TransformToAABB(Matrix const & matrix)
{
#define FindCorner(p)               \
            if (p.x < left)         \
                left = p.x;         \
            if (p.x > right)        \
                right = p.x;        \
            if (p.y < top)          \
                top = p.y;          \
            if (p.y > bottom)       \
                bottom = p.y;

    auto left = std::numeric_limits<double>::max();
    auto right = std::numeric_limits<double>::lowest();
    auto top = std::numeric_limits<double>::max();
    auto bottom = std::numeric_limits<double>::lowest();

    {
        Point point = TopLeft.Transform(matrix);
        FindCorner(point)
    }
    {
        Point point = TopRight.Transform(matrix);
        FindCorner(point)
    }
    {
        Point point = BottomRight.Transform(matrix);
        FindCorner(point)
    }
    {
        Point point = BottomLeft.Transform(matrix);
        FindCorner(point)
    }

    return Rect::xyxy(left, top, right, bottom);
#undef FindCorner
}

/// <summary>
/// Translates the rectangle by an offset.
/// </summary>
/// <param name="offset">The offset.</param>
/// <returns>The translated rectangle.</returns>
Rect XUI::Core::Media::Rect::Translate(Vector const & offset)
{
    return Rect(Point(x0() + offset.x, y0() + offset.y), Size(width(), height()).ToVector());
}

/// <summary>
/// Gets the union of two rectangles.
/// </summary>
/// <param name="rect">The other rectangle.</param>
/// <returns>The union.</returns>
Rect XUI::Core::Media::Rect::Union(Rect const & rect)
{
    auto _x1 = std::min(x0(), rect.x0());
    auto _x2 = std::max(x1(), rect.x1());
    auto _y1 = std::min(y0(), rect.y0());
    auto _y2 = std::max(y1(), rect.y1());

    return Rect::xyxy(_x1, _y1, _x2, _y2);
}

Point XUI::Core::Media::Rect::GetPosition() const
{
    return Point(x0(), y0());
}

/// <summary>
/// Gets the size of the rectangle.
/// </summary>
Size XUI::Core::Media::Rect::GetSize() const
{
    return Size(width(), height());
}

double XUI::Core::Media::Rect::GetLeft() const
{
    return x0();
}

double XUI::Core::Media::Rect::GetTop() const
{
    return y0();
}

double XUI::Core::Media::Rect::GetRight() const
{
    return x0() + width();
}

double XUI::Core::Media::Rect::GetBottom() const
{
    return y0() + height();
}

Point XUI::Core::Media::Rect::GetTopLeft() const
{
    return Point(x0(), y0());
}

Point XUI::Core::Media::Rect::GetTopRight() const
{
    return Point(Right, y0());
}

Point XUI::Core::Media::Rect::GetBottomLeft() const
{
    return Point(x0(), Bottom);
}

Point XUI::Core::Media::Rect::GetBottomRight() const
{
    return Point(Right, Bottom);
}

Point XUI::Core::Media::Rect::GetCenter() const
{
    return Point(x0() + (width() / 2), y0() + (height() / 2));
}

XUI::Core::Media::Rect::Rect(XUI::Core::Media::Size const & size)
    : min(0, 0), max(size.Width, size.Height)
{}
