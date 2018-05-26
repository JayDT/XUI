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

#ifndef _XUI_MEDIA_VECTOR_H__
#define _XUI_MEDIA_VECTOR_H__

#include "Core/Media/MediaSharedDefines.h"

#include <glm/vec2.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

MetaAssemblyBegin
Meta(modul: ReflInclude("Core/Media/Matrix_t.h"))
Meta(modul: ReflInclude("Core/Media/Size_t.h"))
MetaAssemblyEnd

namespace XUI::Core::Media
{
    struct Matrix;
    struct Size;
    struct Vector;

    struct TC_XUI_API Point : glm::vec2, System::Reflection::Object
    {
        Point()
            : glm::vec2(0, 0)
        {}

        Point(float x, float y)
            : glm::vec2(x, y)
        {}

        Point(glm::vec2 const& from)
            : glm::vec2(from)
        {}

        Point(Point const& from)
            : glm::vec2(from)
        {}

        static Point const& Zero()
        {
            static Point zero(0, 0);
            return zero;
        }

        /// <summary>
        /// Parses a color string.
        /// </summary>
        /// <param name="s">The color string.</param>
        /// <returns>The <see cref="Color"/>.</returns>
        static Point Parse(std::string const& s)
        {
            auto parts = System::String(std::move(s)).Trim().Split(',');

            if (parts.size() == 2)
            {
                double horizontal = std::strtod(parts[0].c_str(), nullptr);
                double vertical = std::strtod(parts[1].c_str(), nullptr);
                return Point((float)horizontal, (float)vertical);
            }

            throw System::FormatException("Invalid Thickness.");
        }

        inline Point min(const glm::vec2 &v) const
        {
            return Point(glm::min(v.x, x), glm::min(v.y, y));
        }

        inline Point max(const glm::vec2 &v) const
        {
            return Point(glm::max(v.x, x), glm::max(v.y, y));
        }

        /// <summary>
        /// Transforms the point by a matrix.
        /// </summary>
        /// <param name="transform">The transform.</param>
        /// <returns>The transformed point.</returns>
        Point Transform(Matrix const& transform);

        std::string ToString() const override
        {
            return "";
        }

        Media::Vector ToVector() const;

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(Media::Point))
                return false;

            return *this == *(Media::Point*)(variant.ToPointer());
        }
    };

    struct TC_XUI_API Vector : glm::vec2, System::Reflection::Object
    {
        Vector()
            : glm::vec2(0, 0)
        {}

        Vector(float x, float y)
            : glm::vec2(x, y)
        {}

        Vector(glm::vec2 const& from)
            : glm::vec2(from)
        {}

        Vector(Vector const& from)
            : glm::vec2(from)
        {}

        static Vector const& Zero()
        {
            static Vector zero(0, 0);
            return zero;
        }

        /// <summary>
        /// Parses a color string.
        /// </summary>
        /// <param name="s">The color string.</param>
        /// <returns>The <see cref="Color"/>.</returns>
        static Vector Parse(std::string const& s)
        {
            auto parts = System::String(std::move(s)).Trim().Split(',');

            if (parts.size() == 2)
            {
                double horizontal = std::strtod(parts[0].c_str(), nullptr);
                double vertical = std::strtod(parts[1].c_str(), nullptr);
                return Vector((float)horizontal, (float)vertical);
            }

            throw System::FormatException("Invalid Thickness.");
        }

        inline Vector min(const glm::vec2 &v) const
        {
            return Vector(glm::min(v.x, x), glm::min(v.y, y));
        }

        inline Vector max(const glm::vec2 &v) const
        {
            return Vector(glm::max(v.x, x), glm::max(v.y, y));
        }

        std::string ToString() const override
        {
            return "";
        }

        //! Normalize the vector.
        Vector& Normalize()
        {
            float length = (float)(x*x + y*y);
            if (length == 0)
                return *this;
            length = 1.0f / sqrt(length);
            x = (float)(x * length);
            y = (float)(y * length);
            return *this;
        }

        static float Cross(Vector const& a, Vector  const& b)
        {
            return a.x * b.y - a.y * b.x;
        }

        /// <summary>
        /// Calculates scaling based on a <see cref="Stretch"/> value.
        /// </summary>
        /// <param name="stretch">The stretch mode.</param>
        /// <param name="destinationSize">The size of the destination viewport.</param>
        /// <param name="sourceSize">The size of the source.</param>
        /// <returns>A vector with the X and Y scaling factors.</returns>
        static Vector CalculateScaling(Stretch stretch, Size const& destinationSize, Size const& sourceSize);

        Point ToPoint() const
        {
            return Point(x, y);
        }

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(Media::Vector))
                return false;

            return *this == *(Media::Vector*)(variant.ToPointer());
        }
    };

    inline Media::Vector Point::ToVector() const
    {
        return Media::Vector(x, y);
    }
}

#endif //!_XUI_MEDIA_VECTOR_H__