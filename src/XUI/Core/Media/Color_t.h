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

#ifndef _XUI_MEDIA_COLOR_H__
#define _XUI_MEDIA_COLOR_H__

#include "glm/vec4.hpp"

namespace XUI::Core::Media
{
    struct TC_XUI_API Color : System::Reflection::Object
    {
        glm::vec<4, byte, glm::highp> color;

        Color()
        {}

        Color(const uint8 _r, const uint8 _g, const uint8 _b, const uint8 _a) :
            color(_r, _g, _b, _a) {}

        Color(uint32 i)
        {
            color.a = (i >> 24) & 0xFF;
            color.r = (i >> 16) & 0xFF;
            color.g = (i >> 8) & 0xFF;
            color.b = i & 0xFF;
        }

        static Color FromUInt32(uint32 i)
        {
            return Color(i);
        }

        inline uint32 ToUInt32() const
        {
            return ((uint32)color.a << 24) + ((uint32)color.r << 16) + ((uint32)color.g << 8) + color.b;
        }

        bool operator==(const Color& other) const
        {
            return color == other.color;
        }

        bool operator!=(const Color& other) const
        {
            return color != other.color;
        }

        /// <summary>
        /// Parses a color string.
        /// </summary>
        /// <param name="s">The color string.</param>
        /// <returns>The <see cref="Color"/>.</returns>
        static Color Parse(std::string const& s);

        std::string ToString() const
        {
            return "";
        }

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(Color))
                return false;

            return *this == *(Color*)(variant.ToPointer());
        }
    };
}

#endif //!_XUI_MEDIA_COLOR_H__