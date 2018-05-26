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

#ifndef _XUI_THICKNESS_T_H__
#define _XUI_THICKNESS_T_H__

#include "standard/Platform/Define.h"
#include "Core/Media/Rect_t.h"

namespace XUI::Core::Media
{

    /// <summary>
    /// Describes the thickness of a frame around a rectangle.
    /// </summary>
    struct TC_XUI_API Thickness : System::Reflection::Object
    {
        /// <summary>
        /// Gets the thickness on the left.
        /// </summary>
        double Left;

        /// <summary>
        /// Gets the thickness on the top.
        /// </summary>
        double Top;

        /// <summary>
        /// Gets the thickness on the right.
        /// </summary>
        double Right;

        /// <summary>
        /// Gets the thickness on the bottom.
        /// </summary>
        double Bottom;

        Thickness() : Thickness(0) {}

        /// <summary>
        /// Initializes a new instance of the <see cref="Thickness"/> structure.
        /// </summary>
        /// <param name="uniformLength">The length that should be applied to all sides.</param>
        Thickness(double uniformLength);

        /// <summary>
        /// Initializes a new instance of the <see cref="Thickness"/> structure.
        /// </summary>
        /// <param name="horizontal">The thickness on the left and right.</param>
        /// <param name="vertical">The thickness on the top and bottom.</param>
        Thickness(double horizontal, double vertical);

        /// <summary>
        /// Initializes a new instance of the <see cref="Thickness"/> structure.
        /// </summary>
        /// <param name="left">The thickness on the left.</param>
        /// <param name="top">The thickness on the top.</param>
        /// <param name="right">The thickness on the right.</param>
        /// <param name="bottom">The thickness on the bottom.</param>
        Thickness(double left, double top, double right, double bottom);

        /// <summary>
        /// Gets a value indicating whether all sides are set to 0.
        /// </summary>
        bool IsEmpty() const;

        /// <summary>
        /// Compares two Thicknesses.
        /// </summary>
        /// <param name="a">The first thickness.</param>
        /// <param name="b">The second thickness.</param>
        /// <returns>The equality.</returns>
        bool operator ==(Thickness const& other) const
        {
            return Left == other.Left &&
                Top == other.Top &&
                Right == other.Right &&
                Bottom == other.Bottom;
        }

        /// <summary>
        /// Compares two Thicknesses.
        /// </summary>
        /// <param name="a">The first thickness.</param>
        /// <param name="b">The second thickness.</param>
        /// <returns>The unequality.</returns>
        bool operator !=(Thickness const& b) const
        {
            return !(*this == b);
        }

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(Thickness))
                return false;

            return *this == *(Thickness*)(variant.ToPointer());
        }

        /// <summary>
        /// Adds two Thicknesses.
        /// </summary>
        /// <param name="a">The first thickness.</param>
        /// <param name="b">The second thickness.</param>
        /// <returns>The equality.</returns>
        Thickness operator +(Thickness const& b)
        {
            return Thickness(
                Left + b.Left,
                Top + b.Top,
                Right + b.Right,
                Bottom + b.Bottom);
        }

        /// <summary>
        /// Parses a <see cref="Thickness"/> string.
        /// </summary>
        /// <param name="s">The string.</param>
        /// <param name="culture">The current culture.</param>
        /// <returns>The <see cref="Thickness"/>.</returns>
        static Thickness Parse(std::string const& s);

        /// <summary>
        /// Returns the string representation of the thickness.
        /// </summary>
        /// <returns>The string representation of the thickness.</returns>
        std::string ToString() const;

        /// <summary>
        /// Deflates the rectangle by a <see cref="Thickness"/>.
        /// </summary>
        /// <param name="thickness">The thickness.</param>
        /// <returns>The deflated rectangle.</returns>
        /// <remarks>The deflated rectangle size cannot be less than 0.</remarks>
        void Deflate(Rect& rect);
    };
}

#endif //!_XUI_THICKNESS_T_H__