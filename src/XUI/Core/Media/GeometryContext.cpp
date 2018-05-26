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

#include "GeometryContext.h"
#include "Core/GlobalInitializerScheduler.h"
#include "Core/Media/EllipticalArc.h"
#include "Core/Media/Geometries/PathMarkupParser.h"
#include "Core/Media/Geometries/PathGeometry.h"
#include "Core/Media/Transform.h"

#include "standard/Spline/spline.h"
#include "G3D/Spline.h"
#include "G3D/splinefunc.h"

using namespace XUI;
using namespace XUI::Interfaces;

XUI::Core::Media::GeometryContext::GeometryContext()
{
    m_path.reserve(10);
}

void XUI::Core::Media::GeometryContext::ArcTo(Core::Media::Point const & point, Core::Media::Size const & size, double rotationAngle, bool isLargeArc, SweepDirection sweepDirection)
{
    XUI::Core::Media::ArcTo(this, m_currentPoint, point, size, rotationAngle, isLargeArc, sweepDirection);
    m_currentPoint = point;
}

void XUI::Core::Media::GeometryContext::BeginFigure(Core::Media::Point const & startPoint, bool isFilled)
{
    Bounds = Rect();

    m_filled = isFilled;
    m_currentPoint = startPoint;
    m_nextIndex = 0;

    m_path.emplace_back();
    PathVert& element = m_path.back();
    element.point = startPoint;
    element.type = PathGeometrySplineType::MOVETO;
    m_pathBounds.emplace_back().start = m_path.size() - 1;
}

void XUI::Core::Media::GeometryContext::CubicBezierTo(Core::Media::Point const & point1, Core::Media::Point const & point2, Core::Media::Point const & point3)
{
    Core::Media::Point prevpoint = m_path.back().GetLastPoint();
    m_path.emplace_back();
    PathVert& element = m_path.back();
    element.prev = prevpoint;
    element.point = point1;
    element.point1 = point2;
    element.point2 = point3;
    element.type = PathGeometrySplineType::BEZIER;
}

void XUI::Core::Media::GeometryContext::QuadraticBezierTo(Core::Media::Point const & controlPoint, Core::Media::Point const & endPoint)
{
    //(s, (s + 2c)/ 3, (e + 2c)/ 3, e)
    CubicBezierTo((m_currentPoint + (2.0f * controlPoint)) / 3.0f, (endPoint + (controlPoint * 2.0f)) / 3.0f, endPoint);
}

void XUI::Core::Media::GeometryContext::LineTo(Core::Media::Point const & point)
{
    Core::Media::Point prevpoint = m_path.back().GetLastPoint();
    m_path.emplace_back();
    PathVert& element = m_path.back();
    element.prev = prevpoint;
    element.point = point;
    element.type = PathGeometrySplineType::LINEAR;
}

void XUI::Core::Media::GeometryContext::EndFigure(bool isClosed)
{
    m_pathBounds.back().end = m_path.size() - 1;

    if (isClosed)
    {
        m_primitivBounds.emplace_back().start = m_primitivies.size();
        auto const& itBound = m_pathBounds.back();

        //m_primitivies.reserve(m_nextIndex);
        for (size_t i = itBound.start; i <= itBound.end; ++i)
        {
            auto const& vert = m_path[i];
            switch (vert.type)
            {
                case PathGeometrySplineType::MOVETO:
                    if (m_path[i + 1 % m_path.size()].type != PathGeometrySplineType::LINEAR)
                        break;
                case PathGeometrySplineType::LINEAR:
                {
                    m_primitivies.emplace_back(vert.point);
                    break;
                }
                case PathGeometrySplineType::BEZIER:
                {
                    System::Spline::Spline2D spline;
                    spline.m_mode = System::Spline::Spline2D::ModeBezier3;

                    spline.points.push_back(vert.prev);
                    spline.points.push_back(vert.point);
                    spline.points.push_back(vert.point1);
                    spline.points.push_back(vert.point2);
                    spline.Init();

                    for (int i = spline.index_lo; i < spline.index_hi; ++i)
                    {
                        for (int segoff = 0; segoff != 10; ++segoff)
                        {
                            glm::vec2 p;
                            float segment = float(segoff) / 10.f;
                            spline.EvaluateBezier3(i, segment, p);
                            m_primitivies.emplace_back(p);
                        }
                    }
                    break;
                }
            }

            //if (i == itBound->end)
            //{
            //    m_primitivBounds.back().end = m_primitivies.size() - 1;
            //    if ((i + 1) < m_path.size())
            //        m_primitivBounds.emplace_back().start = m_primitivies.size();
            //}
        }

        m_primitivBounds.back().end = m_primitivies.size() - 1;

        Bounds = XUI::Core::Media::GeometryContext::GetRenderBounds(0.0);
    }
}

XUI::Core::Media::Rect XUI::Core::Media::GeometryContext::GetRenderBounds(double strokeThickness)
{
    if (m_primitivies.empty())
        return Core::Media::Rect();

    Point min =  Point(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
    Point max = Point(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
    for (auto const& path : m_primitivies)
    {
        if (min.x > path.x)
            min.x = path.x;
        if (min.y > path.y)
            min.y = path.y;
        if (max.x < path.x)
            max.x = path.x;
        if (max.y < path.y)
            max.y = path.y;
    }

    return Core::Media::Rect::xyxy(min.x, min.y, max.x, max.y).Inflate(strokeThickness);
}

bool XUI::Core::Media::GeometryContext::FillContains(Core::Media::Point const & point)
{
    // Use the algorithm from http://www.blackpawn.com/texts/pointinpoly/default.html
    // to determine if the point is in the geometry (since it will always be convex in this situation)
    size_t size = m_primitivies.size();
    for (int i = 0; i < size; ++i)
    {
        auto a = m_primitivies[i];
        auto b = m_primitivies[(i + 1) % size];
        auto c = m_primitivies[(i + 2) % size];

        Vector v0 = c - a;
        Vector v1 = b - a;
        Vector v2 = point - a;

        glm::vec2 dot00 = v0 * v0;
        glm::vec2 dot01 = v0 * v1;
        glm::vec2 dot02 = v0 * v2;
        glm::vec2 dot11 = v1 * v1;
        glm::vec2 dot12 = v1 * v2;


        auto invDenom = 1 / (glm::dot(dot00, dot11) - glm::dot(dot01, dot01));
        auto u = (glm::dot(dot11, dot02) - glm::dot(dot01, dot12)) * invDenom;
        auto v = (glm::dot(dot00, dot12) - glm::dot(dot01, dot02)) * invDenom;
        if ((u >= 0) && (v >= 0) && (u + v < 1)) return true;
    }
    return false;
}

XUI::Interfaces::IGeometry * XUI::Core::Media::GeometryContext::Intersect(IGeometry * geometry)
{
    return nullptr;
}

bool XUI::Core::Media::GeometryContext::StrokeContains(Core::Media::Pen const & pen, Core::Media::Point const & point)
{
    return false;
}

XUI::Interfaces::IGeometry * XUI::Core::Media::GeometryContext::WithTransform(Core::Media::Matrix const & transform)
{
    return nullptr;
}
