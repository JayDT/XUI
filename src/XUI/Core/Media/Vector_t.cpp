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

#include "StdAfx.h"
#include "Vector_t.h"
#include "Core/Media/Matrix_t.h"
#include "Core/Media/Size_t.h"

using namespace XUI::Core::Media;

/// <summary>
/// Transforms the point by a matrix.
/// </summary>
/// <param name="transform">The transform.</param>
/// <returns>The transformed point.</returns>
Point XUI::Core::Media::Point::Transform(Matrix const & transform)
{
    //float _x = x;
    //float _y = y;
    //float xadd = y * transform[1][0] + transform[0][3];
    //float yadd = x * transform[0][1] + transform[1][3];
    //_x *= transform[0][0];
    //_x += xadd;
    //_y *= transform[1][1];
    //_y += yadd;
    //return Point(_x, _y);
    return (*this) * (glm::mat4&)transform;
}

/// <summary>
/// Calculates scaling based on a <see cref="Stretch"/> value.
/// </summary>
/// <param name="stretch">The stretch mode.</param>
/// <param name="destinationSize">The size of the destination viewport.</param>
/// <param name="sourceSize">The size of the source.</param>
/// <returns>A vector with the X and Y scaling factors.</returns>
Vector XUI::Core::Media::Vector::CalculateScaling(Stretch stretch, Size const & destinationSize, Size const & sourceSize)
{
    double scaleX = 1;
    double scaleY = 1;

    if (stretch != Stretch::None)
    {
        scaleX = destinationSize.Width >= std::numeric_limits<double>::max() ? 1.0 : destinationSize.Width / sourceSize.Width;
        scaleY = destinationSize.Height >= std::numeric_limits<double>::max() ? 1.0 : destinationSize.Height / sourceSize.Height;

        switch (stretch)
        {
            case Stretch::Uniform:
                scaleX = scaleY = std::min(scaleX, scaleY);
                break;
            case Stretch::UniformToFill:
                scaleX = scaleY = std::max(scaleX, scaleY);
                break;
        }
    }

    return Vector(scaleX, scaleY);
}
