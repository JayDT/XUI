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

#ifndef _XUI_MEDIA_TRIANGULATOR_H__
#define _XUI_MEDIA_TRIANGULATOR_H__

#include "Core/Media/MediaTypes.h"
#include "Core/Media/Geometries/Triangle2D.h"

namespace XUI::Core::Media
{
    //From the Poly2Tri project by Mason Green: http://code.google.com/p/poly2tri/source/browse?repo=archive#hg/scala/src/org/poly2tri/seidel

    struct TC_XUI_API Triangulator final
    {
        /// <summary>
        /// Decompose the polygon into several smaller non-concave polygon.
        /// </summary>
        /// <param name="vertices">The polygon to decompose.</param>
        /// <param name="sheer">The sheer to use. If you get bad results, try using a higher value. The default value is 0.001</param>
        /// <returns>A list of triangles</returns>
        static std::vector<Geometries::Triangle2D> PolyTriangulate(std::vector<Core::Media::Point> &vertices, std::list<std::vector<Core::Media::Point>> &pointsHoles, Core::Media::Point const* pivot, float sheer);

        static bool PolyEarClipGeometry(std::vector<Core::Media::Point> &points, std::list<std::vector<Core::Media::Point>> &pointsHoles, float sheer, Core::Media::Point const* pivot, std::vector<Core::Media::Vector>& verticies, std::vector<uint16>& indicies);
    };
}

#endif //!_XUI_MEDIA_TRIANGULATOR_H__