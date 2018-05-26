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

#ifndef __XUI_RECT_H__
#define __XUI_RECT_H__

#include <standard/BasicPrimities.h>
#include "Core/Media/Vector_t.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("Core/Media/Matrix_t.h"))
Meta(modul: ReflInclude("Core/Media/Thickness_t.h"))
MetaAssemblyEnd

namespace XUI::Core::Media
{
    struct Matrix;
    struct Thickness;
    struct Point;
    struct Vector;
    struct Size;

    /// <summary>
    /// Defines a rectangle.
    /// </summary>
    struct Rect : System::Reflection::Object
    {
    private:

        Vector min, max;

    public:
        static const Rect Empty;

        Rect() : min(0, 0), max(0, 0)
        {}

        Rect(glm::vec2 const& point, glm::vec2 const& size)
            : min(point.x, point.y), max(point.x + size.x, point.y + size.y)
        {
        }

        Rect(double x, double y, double width, double height)
            : min(x, y), max(x + width, y + height)
        {
        }

        Rect(glm::vec2 const& size)
            : min(0, 0), max(size.x, size.y)
        {}

        Rect(Media::Size const& size);

        Rect(Rect const& from)
            : min(from.min), max(from.max)
        {}

        PROPERTY_(double, Width);
        double GetWidth() const { return width(); }
        void SetWidth(double value) { max.x = min.x + value; }

        PROPERTY_(double, Height);
        double GetHeight() const { return height(); }
        void SetHeight(double value) { max.y = min.y + value; }

        PROPERTY_(double, X);
        double GetX() const { return min.x; }
        void SetX(double value) { min.x = value; }

        PROPERTY_(double, Y);
        double GetY() const { return min.y; }
        void SetY(double value) { min.y = value; }

        float width() const
        {
            return max.x - min.x;
        }

        float height() const
        {
            return max.y - min.y;
        }

        float x0() const
        {
            return min.x;
        }

        float x1() const
        {
            return max.x;
        }

        float y0() const
        {
            return min.y;
        }

        float y1() const
        {
            return max.y;
        }

        Vector const& x0y0() const
        {
            return min;
        }

        Vector x1y0() const
        {
            return glm::vec2(max.x, min.y);
        }

        Vector x0y1() const
        {
            return glm::vec2(min.x, max.y);
        }

        Vector const& x1y1() const
        {
            return max;
        }

        Vector wh() const
        {
            return max - min;
        }

        Vector center() const
        {
            return (max + min) * 0.5f;
        }

        float area() const
        {
            return width() * height();
        }

        bool isFinite() const
        {
            auto _0 = glm::isfinite((glm::vec2&)min);
            auto _1 = glm::isfinite((glm::vec2&)max);
            return _0.x && _0.y && _1.x && _1.y;
        }

        void Offset(double x, double y)
        {
            min.x = min.x + x;
            min.y = min.y + y;
            max.x = max.x + x;
            max.y = max.y + y;
        }

        Rect lerp(const Rect& other, float alpha) const
        {
            Rect out;

            out.min = glm::lerp(min, other.min, alpha);
            out.max = glm::lerp(max, other.max, alpha);

            return out;
        }

        static Rect xyxy(float x0, float y0, float x1, float y1)
        {
            Rect r;

            r.min.x = glm::min(x0, x1);
            r.min.y = glm::min(y0, y1);
            r.max.x = glm::max(x0, x1);
            r.max.y = glm::max(y0, y1);

            return r;
        }

        static Rect xyxy(const Vector& v0, const Vector& v1)
        {
            Rect r;

            r.min = v0.min(v1);
            r.max = v0.max(v1);

            return r;
        }

        static Rect xywh(float x, float y, float w, float h)
        {
            return xyxy(x, y, x + w, y + h);
        }

        static Rect xywh(const Vector& v, const Vector& w)
        {
            return xyxy(v.x, v.y, v.x + w.x, v.y + w.y);
        }

        /// <summary>
        /// Parses a color string.
        /// </summary>
        /// <param name="s">The color string.</param>
        /// <returns>The <see cref="Color"/>.</returns>
        static Rect Parse(std::string const& s)
        {
            auto parts = System::String(std::move(s)).Trim().Split(',');

            switch (parts.size())
            {
                case 1:
                {
                    double uniform = std::strtod(parts[0].c_str(), nullptr);
                    return Rect(glm::vec2((float)uniform, (float)uniform));
                }
                case 2:
                {
                    double horizontal = std::strtod(parts[0].c_str(), nullptr);
                    double vertical = std::strtod(parts[1].c_str(), nullptr);
                    return Rect(glm::vec2((float)horizontal, (float)vertical));
                }
                case 4:
                {
                    double left = std::strtod(parts[0].c_str(), nullptr);
                    double top = std::strtod(parts[1].c_str(), nullptr);
                    double right = std::strtod(parts[2].c_str(), nullptr);
                    double bottom = std::strtod(parts[3].c_str(), nullptr);
                    return xyxy(left, top, right, bottom);
                }
            }

            throw System::FormatException("Invalid Thickness.");
        }

        /// <summary>
        /// Deflates the rectangle.
        /// </summary>
        /// <param name="thickness">The thickness.</param>
        /// <returns>The deflated rectangle.</returns>
        /// <remarks>The deflated rectangle size cannot be less than 0.</remarks>
        Rect Deflate(double thickness);

        /// <summary>
        /// Deflates the rectangle by a <see cref="Thickness"/>.
        /// </summary>
        /// <param name="thickness">The thickness.</param>
        /// <returns>The deflated rectangle.</returns>
        /// <remarks>The deflated rectangle size cannot be less than 0.</remarks>
        Rect Deflate(Thickness const& thickness);

        /// Inflates the rectangle.
        /// </summary>
        /// <param name="thickness">The thickness.</param>
        /// <returns>The inflated rectangle.</returns>
        Rect Inflate(Thickness const& thickness);

        /// <summary>
        /// Returns the axis-aligned bounding box of a transformed rectangle.
        /// </summary>
        /// <param name="matrix">The transform.</param>
        /// <returns>The bounding box</returns>
        Rect TransformToAABB(Matrix const& matrix);

        /// <summary>
        /// Translates the rectangle by an offset.
        /// </summary>
        /// <param name="offset">The offset.</param>
        /// <returns>The translated rectangle.</returns>
        Rect Translate(Vector const& offset);

        /// <summary>
        /// Gets the union of two rectangles.
        /// </summary>
        /// <param name="rect">The other rectangle.</param>
        /// <returns>The union.</returns>
        Rect Union(Rect const& rect);

        /// <summary>
        /// Centers another rectangle in this rectangle.
        /// </summary>
        /// <param name="rect">The rectangle to center.</param>
        /// <returns>The centered rectangle.</returns>
        Rect CenterRect(Rect rect)
        {
            return Rect::xywh(x0() + ((width() - rect.width()) / 2), y0() + ((height() - rect.height()) / 2), rect.width(), rect.height());
        }

        /// <summary>
        /// Returns the string representation of the rectangle.
        /// </summary>
        /// <returns>The string representation of the rectangle.</returns>
        std::string ToString() const
        {
            return System::String::format("%f, %f, %f, %f", x0(), y0(), x1(), y1());
        }

        /// <summary>
        /// Gets the position of the rectangle.
        /// </summary>
        PROPERTY_GET_ONLY(Point, Position);
        Point GetPosition() const;

        /// <summary>
        /// Gets the size of the rectangle.
        /// </summary>
        Size GetSize() const;

        /// <summary>
        /// Gets the left position of the rectangle.
        /// </summary>
        PROPERTY_GET_ONLY(double, Left);
        double GetLeft() const;

        /// <summary>
        /// Gets the right position of the rectangle.
        /// </summary>
        PROPERTY_GET_ONLY(double,  Right);
        double GetRight() const;

        /// <summary>
        /// Gets the top position of the rectangle.
        /// </summary>
        PROPERTY_GET_ONLY(double, Top);
        double GetTop() const;

        /// <summary>
        /// Gets the bottom position of the rectangle.
        /// </summary>
        PROPERTY_GET_ONLY(double,  Bottom);
        double GetBottom() const;

        /// <summary>
        /// Gets the top left point of the rectangle.
        /// </summary>
        PROPERTY_GET_ONLY(Point,  TopLeft);
        Point GetTopLeft() const;

        /// <summary>
        /// Gets the top right point of the rectangle.
        /// </summary>
        PROPERTY_GET_ONLY(Point,  TopRight);
        Point GetTopRight() const;

        /// <summary>
        /// Gets the bottom left point of the rectangle.
        /// </summary>
        PROPERTY_GET_ONLY(Point,  BottomLeft);
        Point GetBottomLeft() const;

        /// <summary>
        /// Gets the bottom right point of the rectangle.
        /// </summary>
        PROPERTY_GET_ONLY(Point,  BottomRight);
        Point GetBottomRight() const;

        /// <summary>
        /// Gets the center point of the rectangle.
        /// </summary>
        PROPERTY_GET_ONLY(Point,  Center);
        Point GetCenter() const;

        bool contains(const Point& v) const
        {
            return (v.x >= min.x) && (v.y >= min.y) && (v.x <= max.x) && (v.y <= max.y);
        }

        bool contains(const Vector& v) const
        {
            return (v.x >= min.x) && (v.y >= min.y) && (v.x <= max.x) && (v.y <= max.y);
        }

        bool contains(const Rect& r) const
        {
            return (min.x <= r.min.x) && (min.y <= r.min.y) &&
                (max.x >= r.max.x) && (max.y >= r.max.y);
        }

        bool intersects(const Rect& r) const
        {
            return (min.x < r.max.x) && (min.y < r.max.y) &&
                (max.x > r.min.x) && (max.y > r.min.y);
        }

        Rect intersect(const Rect& other) const
        {
            if (intersects(other))
                return Rect::xyxy(min.max(other.min), max.min(other.max));
            else
                return Rect::xywh(0, 0, 0, 0);
        }

        /** Like intersection, but counts the adjacent case as touching. */
        bool intersectsOrTouches(const Rect& r) const
        {
            return (min.x <= r.max.x) && (min.y <= r.max.y) &&
                (max.x >= r.min.x) && (max.y >= r.min.y);
        }

        Rect operator*(float s) const 
        {
            return xyxy(min.x * s, min.y * s, max.x * s, max.y * s);
        }

        Rect operator/(float s) const
        {
            return xyxy(min / s, max / s);
        }

        Rect operator/(const Vector& s) const
        {
            return xyxy(min / s, max / s);
        }

        Rect operator+(const Vector& v) const
        {
            return xyxy(min + v, max + v);
        }

        Rect operator-(const Vector& v) const
        {
            return xyxy(min - v, max - v);
        }

        bool operator==(const Rect& other) const
        {
            return (min == other.min) && (max == other.max);
        }

        bool operator!=(const Rect& other) const
        {
            return (min != other.min) || (max != other.max);
        }

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(Rect))
                return false;

            return *this == *(Rect*)(variant.ToPointer());
        }
    };
}


#endif	//#ifndef __XUI_RECT_H__
