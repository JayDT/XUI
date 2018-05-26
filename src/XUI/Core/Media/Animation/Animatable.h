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

#ifndef _XUI_ANIMATABLE_H__
#define _XUI_ANIMATABLE_H__

#include "Core/Dependencies/DependencyObject.h"
#include "standard/Platform/Define.h"

namespace XUI::Core::Media::Animation
{
    /// <summary>
    /// Describes the thickness of a frame around a rectangle.
    /// </summary>
    struct TC_XUI_API Animatable : Dependency::DependencyObject
    {
    };
}

#endif //!_XUI_ANIMATABLE_H__