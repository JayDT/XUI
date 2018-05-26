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

#ifndef __XUI_STREAMGEOMETRY_H__
#define __XUI_STREAMGEOMETRY_H__

#include "Core/Media/Geometry.h"

namespace XUI::Core::Media::Geometries
{
    /// <summary>
    /// Represents the geometry of an arbitrarily complex shape.
    /// </summary>
    class StreamGeometry : public Geometry
    {
    public:
        /// <summary>
        /// Initializes a new instance of the <see cref="StreamGeometry"/> class.
        /// </summary>
        StreamGeometry() {}
        virtual ~StreamGeometry() {}
    };
}


#endif	//#ifndef __XUI_STREAMGEOMETRY_H__
