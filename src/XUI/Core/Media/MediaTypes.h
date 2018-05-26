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

#ifndef _XUI_MEDIA_TYPES_H__
#define _XUI_MEDIA_TYPES_H__

#include "Core/Media/Vector_t.h"
#include "Core/Media/Rect_t.h"
#include "Core/Media/Matrix_t.h"
#include "Core/Media/Color_t.h"
#include "Core/Media/TileBrush.h"
#include "Core/Media/Size_t.h"
#include "Core/Media/Thickness_t.h"
#include "Core/Media/MediaSharedDefines.h"
#include "Core/Media/TransformedBound_t.h"

TC_XUI_API glm::vec2 operator*(glm::vec2 const& position, glm::mat4 const& matrix);
TC_XUI_API glm::vec3 operator*(glm::vec3 const& position, glm::mat4 const& matrix);
TC_XUI_API glm::vec4 operator*(glm::vec4 const& position, glm::mat4 const& matrix);
TC_XUI_API glm::mat3x2 operator*(glm::mat3x2 const& m1, glm::mat3x2 const& m2);
TC_XUI_API XUI::Core::Media::Rect operator*(XUI::Core::Media::Rect const& rect, XUI::Core::Media::Matrix const& mat);

#endif //!_XUI_MEDIA_COLOR_H__