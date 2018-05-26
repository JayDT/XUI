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

#include "Triangulator.h"
#include "Wrappers/poly2tri/poly2tri.h"
#include "clipper.hpp"

//#define MPE_POLY2TRI_IMPLEMENTATION
//#include <standard/Geometry/MPE_fastpoly2tri.h>

//void ResetContext(MPEPolyContext* PolyContext, umm IterationRollbackLocation)
//{
//    MPE_Assert(PolyContext->MaxPointCount);
//    u8* EndOfPoints = (u8*)(PolyContext->PointsPool + PolyContext->MaxPointCount);
//    umm SizeOfPoints = sizeof(MPEPolyPoint)*PolyContext->MaxPointCount;
//    umm SizeToClear = PolyContext->Allocator.Size - SizeOfPoints;
//    memset(EndOfPoints, 0, SizeToClear);
//    memset(&PolyContext->Basin, 0, sizeof(PolyContext->Basin));
//    memset(&PolyContext->EdgeEvent, 0, sizeof(PolyContext->EdgeEvent));
//    PolyContext->Allocator.Used = IterationRollbackLocation;
//    PolyContext->TrianglePoolCount = 0;
//    PolyContext->TriangleCount = 0;
//    PolyContext->PointCount = 0;
//    PolyContext->NodeCount = 0;
//
//    // Clear the linked list of edges for each point
//    for (uxx PointIndex = 0; PointIndex < PolyContext->MaxPointCount; ++PointIndex)
//    {
//        MPEPolyPoint* Point = PolyContext->PointsPool + PointIndex;
//        Point->FirstEdge = 0;
//    }
//}
//
//std::vector<XUI::Core::Media::Geometries::Triangle2D> XUI::Core::Media::Triangulator::PolyTriangulate(std::vector<Core::Media::Point>& vertices, Core::Media::Point const* pivot, float sheer)
//{
//    int MaxPointCount = vertices.size() * 10;
//    if (!MaxPointCount)
//        return {};
//
//    std::vector<Geometries::Triangle2D> result;
//
//    MPEPolyContext PolyContext;
//    PolyContext.Valid = 0;
//    umm MemoryRequired = MPE_PolyMemoryRequired(vertices.size() * 16);
//    void* Memory = calloc(MemoryRequired, 1);
//    if (MPE_PolyInitContext(&PolyContext, Memory, MaxPointCount))
//    {
//        umm IterationRollbackLocation = PolyContext.Allocator.Used;
//        for (auto const& p : vertices)
//        {
//            MPEPolyPoint* Point = MPE_PolyPushPoint(&PolyContext);
//            Point->FirstEdge = nullptr;
//            Point->X = (poly_float)p.x;
//            Point->Y = (poly_float)p.y;
//        }
//        //// This option requires your point data structure to
//        //// correspond to MPEPolyPoint. See below.
//        //MPEPolyPoint* FirstPoint = MPE_PolyPushPointArray(&PolyContext, PointCount);
//        //memcpy(FirstPoint, YOUR_POINT_DATA, sizeof(MPEPolyPoint)*PointCount);
//
//        if (PolyContext.Valid)
//        {
//            MPE_PolyAddEdge(&PolyContext);
//            //if (holes)
//            //{
//            //    MPEPolyPoint* Hole = MPE_PolyPushPointArray(&PolyContext, 4);
//            //    Hole[0].X = 325; Hole[0].Y = 437;
//            //    Hole[1].X = 320; Hole[1].Y = 423;
//            //    Hole[2].X = 329; Hole[2].Y = 413;
//            //    Hole[3].X = 332; Hole[3].Y = 423;
//            //    MPE_PolyAddHole(&PolyContext);
//            //}
//            MPE_PolyTriangulate(&PolyContext); // Triangulate the shape
//
//            // The resulting triangles can be used like so
//            for (uxx TriangleIndex = 0; TriangleIndex < PolyContext.TriangleCount; ++TriangleIndex)
//            {
//                MPEPolyTriangle* Triangle = PolyContext.Triangles[TriangleIndex];
//
//                Geometries::Triangle2D tri = pivot == nullptr ?
//                    Geometries::Triangle2D(Triangle->Points[0]->X, Triangle->Points[0]->Y, Triangle->Points[1]->X, Triangle->Points[1]->Y, Triangle->Points[2]->X, Triangle->Points[2]->Y) :
//                    Geometries::Triangle2D(Triangle->Points[0]->X + pivot->x, Triangle->Points[0]->Y + pivot->y, Triangle->Points[1]->X + pivot->x, Triangle->Points[1]->Y + pivot->y, Triangle->Points[2]->X + pivot->x, Triangle->Points[2]->Y + pivot->y);
//
//                result.push_back(std::move(tri));
//            }
//        }
//    }
//    return result;
//}

template <class C> void FreeClear(C & cntr)
{
    for (typename C::iterator it = cntr.begin(); it != cntr.end(); ++it)
        delete * it;
    cntr.clear();
}

std::vector<XUI::Core::Media::Geometries::Triangle2D> XUI::Core::Media::Triangulator::PolyTriangulate(std::vector<Core::Media::Point>& vertices, std::list<std::vector<Core::Media::Point>> &pointsHoles, Core::Media::Point const* pivot, float sheer)
{
    std::vector<Geometries::Triangle2D> result;
    std::vector<p2t::Point*> polyline;
    std::vector<p2t::Point*> polyHoles;
    polyline.reserve(vertices.size());
    ClipperLib::Path path;
    for (Core::Media::Point const& p : vertices)
    {
        bool contains = false;
        for (p2t::Point const* tp : polyline)
        {
            if (tp->x == p.x && tp->y == p.y)
            {
                contains = true;
                break;
            }
        }

        if (!contains)
        {
            ClipperLib::IntPoint _p(p.x, p.y);
            path.push_back(_p);

            polyline.push_back(new p2t::Point(p.x, p.y));
        }
    }

    ClipperLib::Clipper c;
    c.AddPath(path, ClipperLib::PolyType::ptSubject, true);

    for (auto const& shape : pointsHoles)
    {
        ClipperLib::Path cutpath;
        for (Core::Media::Point const& _p : shape)
        {
            ClipperLib::IntPoint p(_p.x, _p.y);
            cutpath.push_back(p);
        }
        c.AddPath(cutpath, ClipperLib::PolyType::ptClip, true);
    }


    ClipperLib::Paths _result;
    c.Execute(ClipperLib::ClipType::ctIntersection, _result);

    polyHoles.reserve(pointsHoles.size());
    for (auto const& _rpath : _result)
    {
        for (auto const& _rpoint : _rpath)
        {
            for (auto itr = polyline.begin(); itr != polyline.end(); ++itr)
            {
                if (uint64((*itr)->x) == _rpoint.X && uint64((*itr)->y) == _rpoint.Y)
                {
                    delete *itr;
                    polyline.erase(itr);
                    break;
                }

            }

            polyHoles.push_back(new p2t::Point(_rpoint.X, _rpoint.Y));
        }
    }

    if (polyline.size() < 3)
    {
        _result.clear();
        c.Execute(ClipperLib::ClipType::ctDifference, _result);

        if (_result.empty())
            return result;

        FreeClear(polyHoles);
        FreeClear(polyline);

        for (auto const& _rpath : _result)
        {
            for (auto const& _rpoint : _rpath)
            {
                polyline.push_back(new p2t::Point(_rpoint.X, _rpoint.Y));
            }
        }

        if (polyline.size() < 3)
            return result;
    }

    /*
    * STEP 1: Create CDT and add primary polyline
    * NOTE: polyline must be a simple polygon. The polyline's points
    * constitute constrained edges. No repeat points!!!
    */
    p2t::CDT cdt(polyline);

    if (!polyHoles.empty())
        cdt.AddHole(polyHoles);

    /*
    * STEP 3: Triangulate!
    */
    cdt.Triangulate();

    std::vector<p2t::Triangle*> triangles = std::move(cdt.GetTriangles());

    // The resulting triangles can be used like so
    result.reserve(triangles.size());
    for (p2t::Triangle* p2tri : triangles)
    {
        Geometries::Triangle2D tri = pivot == nullptr ?
            Geometries::Triangle2D(p2tri->GetPoint(0)->x, p2tri->GetPoint(0)->y, p2tri->GetPoint(1)->x, p2tri->GetPoint(1)->y, p2tri->GetPoint(2)->x, p2tri->GetPoint(2)->y) :
            Geometries::Triangle2D(p2tri->GetPoint(0)->x + pivot->x, p2tri->GetPoint(0)->y + pivot->y, p2tri->GetPoint(1)->x + pivot->x, p2tri->GetPoint(1)->y + pivot->y, p2tri->GetPoint(2)->x + pivot->x, p2tri->GetPoint(2)->y + pivot->y);

        result.push_back(std::move(tri));
    }

    // Free points
    FreeClear(polyline);
    FreeClear(polyHoles);

    return result;
}

bool XUI::Core::Media::Triangulator::PolyEarClipGeometry(std::vector<Core::Media::Point> &points, std::list<std::vector<Core::Media::Point>> &pointsHoles, float sheer, Core::Media::Point const* pivot, std::vector<Core::Media::Vector>& verticies, std::vector<uint16>& indicies)
{
    std::vector<XUI::Core::Media::Geometries::Triangle2D> triangles = std::move(PolyTriangulate(points, pointsHoles, pivot, sheer));

    for (int i = 0; i != triangles.size(); ++i)
    {
        auto const& tri = triangles[i];
        short ix[3] = { -1, -1, -1 };

        for (short iv = 0; iv != verticies.size(); ++iv)
        {
            Vector const& vec = verticies[iv];
            if (ix[0] == -1 && fabs(tri.X[0] - vec.x) <= 0.0001 && fabs(tri.Y[0] - vec.y) <= 0.0001)
                ix[0] = iv;
            if (ix[1] == -1 && fabs(tri.X[1] - vec.x) <= 0.0001 && fabs(tri.Y[1] - vec.y) <= 0.0001)
                ix[1] = iv;
            if (ix[2] == -1 && fabs(tri.X[2] - vec.x) <= 0.0001 && fabs(tri.Y[2] - vec.y) <= 0.0001)
                ix[2] = iv;
        }

        if (ix[0] == -1)
        {
            indicies.push_back((short)verticies.size());
            verticies.emplace_back(tri.X[0], tri.Y[0]);
        }
        else
            indicies.push_back(ix[0]);

        if (ix[1] == -1)
        {
            indicies.push_back((short)verticies.size());
            verticies.emplace_back(tri.X[1], tri.Y[1]);
        }
        else
            indicies.push_back(ix[1]);

        if (ix[2] == -1)
        {
            indicies.push_back((short)verticies.size());
            verticies.emplace_back(tri.X[2], tri.Y[2]);
        }
        else
            indicies.push_back(ix[2]);
    }

    return true;
}
