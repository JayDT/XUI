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

#ifndef _XUI_IGEOMERTY_H__
#define _XUI_IGEOMERTY_H__

#include "standard/BasicPrimities.h"

namespace XUI::Core::Media
{
    class Pen;
    class Transform;

    struct Point;
    struct Vector;
    struct Rect;
    struct Size;
    struct Matrix;
}

namespace XUI::Interfaces
{
    struct IStreamGeometry;
    struct IStreamGeometryContext;

    enum class Meta(Enable) FillRule
    {
        EvenOdd,
        NonZero
    };

    /// <summary>
    /// Defines the direction an which elliptical arc is drawn.
    /// </summary>
    enum class Meta(Enable) SweepDirection
    {
        /// <summary>
        /// Specifies that arcs are drawn in a counter clockwise (negative-angle) direction.
        /// </summary>
        CounterClockwise,

        /// <summary>
        /// Specifies that arcs are drawn in a clockwise (positive-angle) direction.
        /// </summary>
        Clockwise,
    };

    /// <summary>
    /// Defines the platform-specific interface for <see cref="Avalonia.Media.Geometry"/>.
    /// </summary>
    struct
        Meta(ReflInclude("Core/Media/Pen.h"))
        TC_XUI_API
        IGeometry
    {
        /// <summary>
        /// Gets the geometry's bounding rectangle.
        /// </summary>
        PURE_VIRTUAL_R_PROPERTY(Core::Media::Rect, Bounds);

        /// <summary>
        /// Gets the transform to applied to the geometry.
        /// </summary>
        virtual std::shared_ptr<Core::Media::Transform> GetTransform() const = 0;
        virtual void SetTransform(std::shared_ptr<XUI::Core::Media::Transform> const& value) = 0;

        /// <summary>
        /// Gets the geometry's bounding rectangle with the specified stroke thickness.
        /// </summary>
        /// <param name="strokeThickness">The stroke thickness.</param>
        /// <returns>The bounding rectangle.</returns>
        virtual Core::Media::Rect GetRenderBounds(double strokeThickness) = 0;

        /// <summary>
        /// Indicates whether the geometry's fill contains the specified point.
        /// </summary>
        /// <param name="point">The point.</param>
        /// <returns><c>true</c> if the geometry contains the point; otherwise, <c>false</c>.</returns>
        virtual bool FillContains(Core::Media::Point const& point) = 0;

        /// <summary>
        /// Intersects the geometry with another geometry.
        /// </summary>
        /// <param name="geometry">The other geometry.</param>
        /// <returns>A new <see cref="IGeometryImpl"/> representing the intersection.</returns>
        virtual IGeometry* Intersect(IGeometry* geometry) = 0;

        /// <summary>
        /// Indicates whether the geometry's stroke contains the specified point.
        /// </summary>
        /// <param name="pen">The stroke to use.</param>
        /// <param name="point">The point.</param>
        /// <returns><c>true</c> if the geometry contains the point; otherwise, <c>false</c>.</returns>
        virtual bool StrokeContains(Core::Media::Pen const& pen, Core::Media::Point const& point) = 0;

        /// <summary>
        /// Makes a clone of the geometry with the specified transform.
        /// </summary>
        /// <param name="transform">The transform.</param>
        /// <returns>The cloned geometry.</returns>
        virtual IGeometry* WithTransform(Core::Media::Matrix const& transform) = 0;

        virtual IStreamGeometry* GetStreamGeometryContext() const = 0;
    }; 

    /// <summary>
    /// Defines the platform-specific interface for a <see cref="Avalonia.Media.StreamGeometry"/>.
    /// </summary>
    struct TC_XUI_API IStreamGeometry : IGeometry
    {
        /// <summary>
        /// Clones the geometry.
        /// </summary>
        /// <returns>A cloned geometry.</returns>
        //virtual IStreamGeometry* Clone() = 0;

        /// <summary>
        /// Opens the geometry to start defining it.
        /// </summary>
        /// <returns>
        /// An <see cref="IStreamGeometryContextImpl"/> which can be used to define the geometry.
        /// </returns>
        virtual struct IStreamGeometryContext* Open() = 0;
    };

    /// <summary>
    /// Describes a geometry using drawing commands.
    /// </summary>
    struct TC_XUI_API IStreamGeometryContext : System::Reflection::Object
    {
        virtual Core::Media::Transform* GetRefTransform() const = 0;
        virtual void SetRefTransform(Core::Media::Transform* value) = 0;

        /// <summary>
        /// Draws an arc to the specified point.
        /// </summary>
        /// <param name="point">The destination point.</param>
        /// <param name="size">The radii of an oval whose perimeter is used to draw the angle.</param>
        /// <param name="rotationAngle">The rotation angle of the oval that specifies the curve.</param>
        /// <param name="isLargeArc">true to draw the arc greater than 180 degrees; otherwise, false.</param>
        /// <param name="sweepDirection">
        /// A value that indicates whether the arc is drawn in the Clockwise or Counterclockwise direction.
        /// </param>
        virtual void ArcTo(Core::Media::Point const& point, Core::Media::Size const& size, double rotationAngle, bool isLargeArc, SweepDirection sweepDirection) = 0;

        /// <summary>
        /// Begins a new figure.
        /// </summary>
        /// <param name="startPoint">The starting point for the figure.</param>
        /// <param name="isFilled">Whether the figure is filled.</param>
        virtual void BeginFigure(Core::Media::Point const& startPoint, bool isFilled) = 0;

        /// <summary>
        /// Draws a Bezier curve to the specified point.
        /// </summary>
        /// <param name="point1">The first control point used to specify the shape of the curve.</param>
        /// <param name="point2">The second control point used to specify the shape of the curve.</param>
        /// <param name="point3">The destination point for the end of the curve.</param>
        virtual void CubicBezierTo(Core::Media::Point const& point1, Core::Media::Point const& point2, Core::Media::Point const& point3) = 0;

        /// <summary>
        /// Draws a quadratic Bezier curve to the specified point
        /// </summary>
        /// <param name="control">Control point</param>
        /// <param name="endPoint">DestinationPoint</param>
        virtual void QuadraticBezierTo(Core::Media::Point const& control, Core::Media::Point const& endPoint) = 0;

        /// <summary>
        /// Draws a line to the specified point.
        /// </summary>
        /// <param name="point">The destination point.</param>
        virtual void LineTo(Core::Media::Point const& point) = 0;

        /// <summary>
        /// Ends the figure started by <see cref="BeginFigure(Point, bool)"/>.
        /// </summary>
        /// <param name="isClosed">Whether the figure is closed.</param>
        virtual void EndFigure(bool isClosed) = 0;

        virtual void SetFillRule(FillRule fillRule) = 0;
    };
}

#endif // !_XUI_IBITMAP_H__
