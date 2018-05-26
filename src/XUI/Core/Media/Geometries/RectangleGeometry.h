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

#ifndef _XUI_MEDIA_RECT_GEOMETRY_H__
#define _XUI_MEDIA_RECT_GEOMETRY_H__

#include "Core/Dependencies/Models/DataProperty.h"
#include "Core/Media/Geometries/PathGeometry.h"

namespace XUI::Core::Media::Geometries
{
    class TC_XUI_API RectangleGeometry : public Geometry
    {
    public:
        RectangleGeometry() {}
        RectangleGeometry(Rect const& rect);
    };
}

#endif //!_XUI_MEDIA_RECT_GEOMETRY_H__