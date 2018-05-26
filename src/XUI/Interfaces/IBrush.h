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

#ifndef _XUI_IBRUSH_H__
#define _XUI_IBRUSH_H__

#include "Core/Dependencies/Models/DataProperty.h"
#include <standard/misc.h>

namespace XUI::Interfaces
{
    enum class BurshTypes
    {
        BURSH = 0,
        SOLID_COLOR = 1,
        GRADIENT_BRUSH = 2,
        LINEAR_GRADIENT_BRUSH = 3,
        RADIAL_GRADIENT_BRUSH = 4,
        TILE_BRUSH = 5,
        VISUAL_BRUSH = 5,
    };

    /// <summary>
    /// Describes how an area is painted.
    /// </summary>
    struct IBrush : public Core::Dependency::DependencyObject
    {
        virtual ~IBrush() {}

        /// <summary>
        /// Gets the type of the brush.
        /// </summary>
        virtual BurshTypes GetTypeId() const = 0;

        /// <summary>
        /// Gets the opacity of the brush.
        /// </summary>
        virtual double GetOpacity() const = 0;
    };
}

#endif // !_XUI_IBITMAP_H__
