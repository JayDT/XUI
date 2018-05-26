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

#ifndef _XUI_MEDIA_RELATIVE_POINT_H__
#define _XUI_MEDIA_RELATIVE_POINT_H__

#include "standard/BasicPrimities.h"
#include "Core/Media/MediaTypes.h"
#include "Core/Media/Size_t.h"

namespace XUI::Core::Media
{
    /// <summary>
    /// Defines the reference point units of an <see cref="RelativePoint"/> or 
    /// <see cref="RelativeRect"/>.
    /// </summary>
    enum class RelativeUnit
    {
        /// <summary>
        /// The point is expressed as a fraction of the containing element's size.
        /// </summary>
        Relative,

        /// <summary>
        /// The point is absolute (i.e. in pixels).
        /// </summary>
        Absolute,
    };

    /// <summary>
    /// Defines a point that may be defined relative to a containing element.
    /// </summary>
    /// <summary>
    /// Defines a point that may be defined relative to a containing element.
    /// </summary>
    struct Meta(Enable) Meta(AbstractType) RelativePoint
    {
    private:
        Core::Media::Point _point;
        RelativeUnit _unit;

    public:
        /// <summary>
        /// A point at the top left of the containing element.
        /// </summary>
        static const RelativePoint TopLeft;

        /// <summary>
        /// A point at the center of the containing element.
        /// </summary>
        static const RelativePoint Center;

        /// <summary>
        /// A point at the bottom right of the containing element.
        /// </summary>
        static const RelativePoint BottomRight;

        RelativePoint() : RelativePoint(RelativePoint::Center) {}

        /// <summary>
        /// Initializes a new instance of the <see cref="RelativePoint"/> struct.
        /// </summary>
        /// <param name="point">The point.</param>
        /// <param name="unit">The unit.</param>
        RelativePoint(Point point, RelativeUnit unit);

        /// <summary>
        /// Initializes a new instance of the <see cref="RelativePoint"/> struct.
        /// </summary>
        /// <param name="x">The X point.</param>
        /// <param name="y">The Y point</param>
        /// <param name="unit">The unit.</param>
        RelativePoint(double x, double y, RelativeUnit unit);

        /// <summary>
        /// Gets the point.
        /// </summary>
        PROPERTY_GS(_point, Point);

        /// <summary>
        /// Gets the unit.
        /// </summary>
        PROPERTY_G(_unit, Unit);

        /// <summary>
        /// Checks for equality between two <see cref="RelativePoint"/>s.
        /// </summary>
        /// <param name="left">The first point.</param>
        /// <param name="right">The second point.</param>
        /// <returns>True if the points are equal; otherwise false.</returns>
        bool operator ==(RelativePoint const& right)
        {
            return Unit == const_cast<RelativePoint&>(right).Unit && Point == const_cast<RelativePoint&>(right).Point;
        }

        /// <summary>
        /// Checks for unequality between two <see cref="RelativePoint"/>s.
        /// </summary>
        /// <param name="left">The first point.</param>
        /// <param name="right">The second point.</param>
        /// <returns>True if the points are unequal; otherwise false.</returns>
        bool operator !=(RelativePoint right)
        {
            return !(*this == right);
        }

        /// <summary>
        /// Gets a hashcode for a <see cref="RelativePoint"/>.
        /// </summary>
        /// <returns>A hash code.</returns>
        //int GetHashCode()
        //{
        //    int hash = 17;
        //    hash = (hash * 23) + Unit.GetHashCode();
        //    hash = (hash * 23) + Point.GetHashCode();
        //    return hash;
        //}

        /// <summary>
        /// Converts a <see cref="RelativePoint"/> into pixels.
        /// </summary>
        /// <param name="size">The size of the visual.</param>
        /// <returns>The origin point in pixels.</returns>
        Core::Media::Point ToPixels(Core::Media::Size size);

        /// <summary>
        /// Parses a <see cref="RelativePoint"/> string.
        /// </summary>
        /// <param name="s">The string.</param>
        /// <param name="culture">The current culture.</param>
        /// <returns>The parsed <see cref="RelativePoint"/>.</returns>
        static RelativePoint Parse(std::string const& s);

        std::string ToString() const
        {
            return "";
        }
    };
}

#endif //!_XUI_MEDIA_RELATIVE_POINT_H__