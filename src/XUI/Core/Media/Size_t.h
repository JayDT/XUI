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

#ifndef _XUI_SIZE_T_H__
#define _XUI_SIZE_T_H__

#include "standard/Platform/Define.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("Core/Media/MediaTypes.h"))
MetaAssemblyEnd

namespace XUI::Core::Media
{
    enum class Stretch : uint8;
    struct Thickness;
    struct Vector;

    /// <summary>
    /// Defines a size.
    /// </summary>
    struct TC_XUI_API Size : System::Reflection::Object
    {
        /// <summary>
        /// A size representing infinity.
        /// </summary>
        static const Size Infinity;

        /// <summary>
        /// A size representing zero
        /// </summary>
        static const Size Empty;

        /// <summary>
        /// The width.
        /// </summary>
        double Width;

        /// <summary>
        /// The height.
        /// </summary>
        double Height;

        Size() : Size(0, 0) {}

        /// <summary>
        /// Initializes a new instance of the <see cref="Size"/> structure.
        /// </summary>
        /// <param name="width">The width.</param>
        /// <param name="height">The height.</param>
        Size(double width, double height);

        /// <summary>
        /// Gets the aspect ratio of the size.
        /// </summary>
        double AspectRatio() const { return Width / Height; }

        /// <summary>
        /// Checks for equality between two <see cref="Size"/>s.
        /// </summary>
        /// <param name="left">The first size.</param>
        /// <param name="right">The second size.</param>
        /// <returns>True if the sizes are equal; otherwise false.</returns>
        bool operator ==(Size const& right) const
        {
            return Width == right.Width && Height == right.Height;
        }

        /// <summary>
        /// Checks for unequality between two <see cref="Size"/>s.
        /// </summary>
        /// <param name="left">The first size.</param>
        /// <param name="right">The second size.</param>
        /// <returns>True if the sizes are unequal; otherwise false.</returns>
        bool operator !=(Size const& right) const
        {
            return !(*this == right);
        }

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(Size))
                return false;

            return *this == *(Size*)(variant.ToPointer());
        }

        /// <summary>
        /// Scales a size.
        /// </summary>
        /// <param name="size">The size</param>
        /// <param name="scale">The scaling factor.</param>
        /// <returns>The scaled size.</returns>
        Size operator *(Vector const& scale);

        /// <summary>
        /// Scales a size.
        /// </summary>
        /// <param name="size">The size</param>
        /// <param name="scale">The scaling factor.</param>
        /// <returns>The scaled size.</returns>
        Size operator /(Vector const& scale);

        /// <summary>
        /// Divides a size by another size to produce a scaling factor.
        /// </summary>
        /// <param name="left">The first size</param>
        /// <param name="right">The second size.</param>
        /// <returns>The scaled size.</returns>
        Vector operator /(Size const& right);

        /// <summary>
        /// Scales a size.
        /// </summary>
        /// <param name="size">The size</param>
        /// <param name="scale">The scaling factor.</param>
        /// <returns>The scaled size.</returns>
        Size operator *(double scale)
        {
            return Size(Width * scale, Height * scale);
        }

        /// <summary>
        /// Scales a size.
        /// </summary>
        /// <param name="size">The size</param>
        /// <param name="scale">The scaling factor.</param>
        /// <returns>The scaled size.</returns>
        Size operator /(double scale)
        {
            return Size(Width / scale, Height / scale);
        }

        Size operator +(Size toAdd)
        {
            return Size(Width + toAdd.Width, Height + toAdd.Height);
        }

        Size operator -(Size toSubstract)
        {
            return Size(Width - toSubstract.Width, Height - toSubstract.Height);
        }

        /// <summary>
        /// Adds a Thickness to a Size.
        /// </summary>
        /// <param name="size">The size.</param>
        /// <param name="thickness">The thickness.</param>
        /// <returns>The equality.</returns>
        Size operator +(Thickness const& thickness);

        /// <summary>
        /// Subtracts a Thickness from a Size.
        /// </summary>
        /// <param name="size">The size.</param>
        /// <param name="thickness">The thickness.</param>
        /// <returns>The equality.</returns>
        Size operator -(Thickness const& thickness);

        /// <summary>
        /// Parses a <see cref="Size"/> string.
        /// </summary>
        /// <param name="s">The string.</param>
        /// <param name="culture">The current culture.</param>
        /// <returns>The <see cref="Size"/>.</returns>
        static Size Parse(std::string const& s);

        /// <summary>
        /// Constrains the size.
        /// </summary>
        /// <param name="constraint">The size to constrain to.</param>
        /// <returns>The constrained size.</returns>
        Size Constrain(Size const& constraint) const;

        /// <summary>
        /// Deflates the size by a <see cref="Thickness"/>.
        /// </summary>
        /// <param name="thickness">The thickness.</param>
        /// <returns>The deflated size.</returns>
        /// <remarks>The deflated size cannot be less than 0.</remarks>
        Size Deflate(Thickness const& thickness) const;

        /// <summary>
        /// Inflates the size by a <see cref="Thickness"/>.
        /// </summary>
        /// <param name="thickness">The thickness.</param>
        /// <returns>The inflated size.</returns>
        /// <summary>
        Size Inflate(Thickness const& thickness) const;

        /// <summary>
        /// Returns a new <see cref="Size"/> with the same height and the specified width.
        /// </summary>
        /// <param name="width">The width.</param>
        /// <returns>The new <see cref="Size"/>.</returns>
        Size WithWidth(double width);

        /// <summary>
        /// Returns a new <see cref="Size"/> with the same width and the specified height.
        /// </summary>
        /// <param name="height">The height.</param>
        /// <returns>The new <see cref="Size"/>.</returns>
        Size WithHeight(double height);

        static Size CalculateSize(Core::Media::Stretch stretch, Size destinationSize, Size sourceSize);

        /// <summary>
        /// Returns the string representation of the size.
        /// </summary>
        /// <returns>The string representation of the size.</returns>
        std::string ToString() const;

        Core::Media::Vector ToVector() const;
    };
}

#endif //!_XUI_SIZE_T_H__