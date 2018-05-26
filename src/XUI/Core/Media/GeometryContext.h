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

#ifndef _XUI_MEDIA_GEOMETRY_CONTEXT_H__
#define _XUI_MEDIA_GEOMETRY_CONTEXT_H__

#include "Interfaces/IGeometry.h"
#include "Core/Media/Vector_t.h"
#include "Core/Media/Rect_t.h"

namespace XUI::Core::Media
{
    struct Transform;

    enum class PathGeometrySplineType : uint8
    {
        LINEAR = 0,
        BEZIER,
        MOVETO,
    };

    struct PathVert
    {
        glm::vec2 prev;
        glm::vec2 point;
        glm::vec2 point1;
        glm::vec2 point2;
        PathGeometrySplineType type;

        glm::vec2 const& GetLastPoint() const
        {
            if (type == PathGeometrySplineType::BEZIER)
                return point2;
            return point;
        }
    };

    struct PathFigure
    {
        int16 start;
        int16 end;
    };

    /// <summary>
    /// Defines a geometric shape.
    /// </summary>
    class
        Meta(ReflInclude("Core/Media/Pen.h"))
        TC_XUI_API Meta(AbstractType)
        GeometryContext : public Interfaces::IStreamGeometryContext
    {
    public:
        typedef std::vector<PathVert> Path;
        typedef std::vector<Media::Point> Primitivies;
        typedef std::vector<PathFigure> PathBound;

    protected:

        Path m_path;
        PathBound m_pathBounds;

        Rect _bounds;

        Primitivies m_primitivies;
        PathBound m_primitivBounds;

        Point m_currentPoint;
        uint16 m_nextIndex = 0;

        bool m_filled = false;

        Core::Media::Transform* m_transform = nullptr;

    public:

        GeometryContext();

        /// <summary>
        /// Gets the geometry's bounding rectangle.
        /// </summary>
        PROPERTY_T_GS(Rect, _bounds, Bounds)

        /// <summary>
        /// Gets or sets a transform to apply to the geometry.
        /// </summary>
        PROPERTY_T(Core::Media::Transform*, RefTransform)
        Core::Media::Transform* GetRefTransform() const override { return m_transform; }
        void SetRefTransform(Core::Media::Transform* value) override { m_transform = value; }

        // Inherited via Geometry
        virtual void ArcTo(Core::Media::Point const & point, Core::Media::Size const & size, double rotationAngle, bool isLargeArc, Interfaces::SweepDirection sweepDirection) override;
        virtual void BeginFigure(Core::Media::Point const & startPoint, bool isFilled) override;
        virtual void CubicBezierTo(Core::Media::Point const & point1, Core::Media::Point const & point2, Core::Media::Point const & point3) override;
        virtual void QuadraticBezierTo(Core::Media::Point const & control, Core::Media::Point const & endPoint) override;

        // Inherited via IStreamGeometryContext
        virtual void LineTo(Core::Media::Point const & point) override;
        virtual void EndFigure(bool isClosed) override;
        virtual void SetFillRule(Interfaces::FillRule fillRule) override {}

        // If not have native handle
        virtual Core::Media::Rect GetRenderBounds(double strokeThickness);
        virtual bool FillContains(Core::Media::Point const & point);
        virtual Interfaces::IGeometry * Intersect(Interfaces::IGeometry * geometry);
        virtual bool StrokeContains(Core::Media::Pen const & pen, Core::Media::Point const & point);
        virtual Interfaces::IGeometry * WithTransform(Core::Media::Matrix const & transform);
    };
}
#endif //!_XUI_MEDIA_GEOMETRY_CONTEXT_H__